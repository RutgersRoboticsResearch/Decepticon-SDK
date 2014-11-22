#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <pthread.h>
#include "serial.h"

#define INPUT_DIR "/dev/"
static char *PREFIXES[3] = {
  "ttyACM",
  "ttyUSB",
  NULL
};

static void *_serial_update(void *connection);
static int setSerAttr(struct serial_t *connection);
static char tempbuf[SWREADMAX];

/** Connect to a serial device.
 *  @param connection
 *    a pointer to the serial struct
 *  @param port
 *    a portname; if NULL, will open a random port
 *  @param baudrate
 *    the bits per second of information to transmit/receive
 *  @param parity
 *    specifies whether or not parity is turned on (if unsure, use 0)
 *  @return 0 on success, -1 on failure
 */
int serial_connect(struct serial_t *connection, char *port, int baudrate, int parity) {
  if (port) {
    connection->port = (char *)malloc((strlen(port) + 1) * sizeof(char));
    strcpy(connection->port, port);
    if ((connection->fd = open(connection->port, O_RDWR)) == -1)
      goto error;
  } else {
    DIR *dp;
    struct dirent *ent;
    char hasPossibleSerial;
    if (!(dp = opendir(INPUT_DIR))) {
      fprintf(stderr, "Cannot find directory %s to open serial connection\n", INPUT_DIR);
      return -1;
    }
    connection->connected = 0;
    while ((ent = readdir(dp))) {
      char *prefix;
      int i;
      hasPossibleSerial = 0;
      for (prefix = PREFIXES[(i = 0)]; prefix != NULL; prefix = PREFIXES[++i])
        if (strstr(ent->d_name, prefix)) {
          connection->port = (char *)malloc((strlen(INPUT_DIR) + strlen(ent->d_name) + 1) * sizeof(char));
          sprintf(connection->port, "%s%s", INPUT_DIR, ent->d_name);
          if ((connection->fd = open(connection->port, O_RDWR | O_NOCTTY | O_NDELAY)) == -1) {
            free(connection->port);
            connection->port = NULL;
          } else {
            hasPossibleSerial = 1;
            break;
          }
        }
      if (hasPossibleSerial)
        break;
    }
    if (!hasPossibleSerial) {
      fprintf(stderr, "Cannot find a serial device to open\n");
      return -1;
    }
  }

  /* set connection attributes */
  connection->baudrate = baudrate;
  connection->parity = parity;
  if (setSerAttr(connection) == -1)
    goto error; /* possible bad behavior */
  tcflush(connection->fd, TCIFLUSH);
  connection->connected = 1;
  memset(connection->buffer, 0, SWBUFMAX);
  memset(connection->readbuf, 0, SWREADMAX);
  connection->readAvailable = 0;

  /* start update thread */
  if (pthread_create(&connection->thread, NULL, _serial_update, (void *)connection) != 0)
    goto error; /* possible bad behavior */
  connection->alive = 1;
  return 0;

error:
  fprintf(stderr, "Cannot connect to the device on %s\n", connection->port);
  connection->connected = 0;
  connection->alive = 0;
  if (connection->fd != -1)
    close(connection->fd);
  connection->fd = -1;
  if (connection->port)
    free(connection->port);
  connection->port = NULL;
  return -1;
}

/** Helper method to set the attributes of a serial connection,
 *  particularly for the arduino.
 *  @param connection
 *    the serial port to connect to
 *  @return 0 on success, -1 on failure
 */
static int setSerAttr(struct serial_t *connection) {
  struct termios tty;
  cfsetospeed(&tty, connection->baudrate);
  cfsetispeed(&tty, connection->baudrate);
  if (tcgetattr(connection->fd, &tty) == -1)
    return -1;
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_oflag &= ~OPOST;
  tty.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
  tty.c_cflag |= CS8 | CLOCAL | CREAD;
  tty.c_cflag &= ~CRTSCTS;
  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  tty.c_cc[VMIN] = 0;
  tty.c_cc[VTIME] = 5;
  if (tcsetattr(connection->fd, TCSANOW, &tty) == -1)
    return -1;
  return 0;
}

/** Threadable method to update the readbuf of the serial communication,
 *  as well as the connection itself.
 *  @param connection_arg
 *    the serial struct defined opaquely for thread function usage
 *  @return 0 on exit
 *  @note
 *    the packets will be read in the following format:
 *    data\n
 *    however, the \n will be cut off
 */
static void *_serial_update(void *connection_arg) {
  struct serial_t *connection;
  int numAvailable;
  int totalBytes;

  connection = (struct serial_t *)connection_arg;
  while (connection->alive) {
    /* dynamically reconnect the device */
    if (access(connection->port, O_RDWR) != 0) {
      if (connection->connected) {
        connection->connected = 0;
        connection->fd = -1;
      }
    } else {
      if (!connection->connected) {
        if ((connection->fd = open(connection->port, O_RDWR | O_NOCTTY | O_NDELAY)) != -1) {
          if (setSerAttr(connection) == 0) {
            connection->connected = 1;
          } else {
            close(connection->fd);
            connection->fd = -1;
          }
        }
      }
    }
    if (!connection->connected)
      continue;

    usleep(10 * 1E3);

    /* update buffer */
    if ((numAvailable = read(connection->fd, tempbuf, SWREADMAX)) > 0) {
      char *start_index, *end_index;
      tempbuf[numAvailable] = '\0';
      if ((totalBytes = strlen(connection->buffer) + numAvailable) >= SWBUFMAX) {
        totalBytes -= SWBUFMAX - 1;
        memmove(connection->buffer, &connection->buffer[totalBytes],
            (SWBUFMAX - totalBytes) * sizeof(char));
        connection->buffer[SWBUFMAX - totalBytes] = '\0';
      }
      strcat(connection->buffer, tempbuf);

      /* get next message packet */
      if ((end_index = strrchr(connection->buffer, '\n'))) {
        end_index[0] = '\0';
        start_index = strrchr(connection->buffer, '\n');
        start_index = start_index ? &start_index[1] : connection->buffer;
        end_index = &end_index[1];
        memcpy(connection->readbuf, start_index,
            (strlen(start_index) + 1) * sizeof(char));
        memmove(connection->buffer, end_index,
            (strlen(end_index) + 1) * sizeof(char));
        connection->readAvailable = 1;
      }
    }
  }
  pthread_exit(NULL);
  return NULL;
}

/** Read a string from the serial communication link.
 *  @param connection
 *    the serial connection to read a message from
 *  @return the readbuf
 */
char *serial_read(struct serial_t *connection) {
  return connection->readbuf;
}

/** Write a message to the serial communication link.
 *  @param connection
 *    the serial communication link to write to
 *  @param message
 *    the message to send over to the other side
 */
void serial_write(struct serial_t *connection, char *message) {
  if (connection->fd != -1)
    write(connection->fd, message, strlen(message));
}

/** Disconnect from the USB Serial port.
 *  @param connection
 *    A pointer to the serial struct.
 */
void serial_disconnect(struct serial_t *connection) {
  if (connection->alive) {
    connection->alive = 0;
    pthread_join(connection->thread, NULL);
  }

  /* clean up */
  if (!connection->connected)
    return;
  if (connection->fd != -1)
    close(connection->fd);
  if (connection->port != NULL)
    free(connection->port);
  memset(connection, 0, sizeof(struct serial_t));
  connection->fd = -1;
}
