#include <termios.h>
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

/** Connect to a serial device.
 *  @param connection
 *    A pointer to the serial struct.
 *  @param port
 *    A portname, if specified. If not, or if NULL, will open a random port.
 *  @param baudrate
 *    A baudrate, defaulted to 9600.
 *  @param parity
 *    A bool to decide whether or not parity to be turned on.
 */
int serial_connect(struct serial_t *connection, char *port, int baudrate, int parity) {
  /* try to open the device */
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
          if ((connection->fd = open(connection->port, O_RDWR)) == -1) {
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
      fprintf(stderr, "Cannot find a device to open\n");
      return -1;
    }
  }

  /* set connection attributes */
  connection->baudrate = baudrate;
  connection->parity = parity;
  if (setSerAttr(connection) == -1)
    goto error; /* possible bad behavior */
  connection->connected = 1;
  connection->id = NULL;
  memset(connection->buffer, 0, SWBUFMAX);
  memset(connection->readbuf, 0, SWREADMAX);
  memset(connection->writebuf, 0, SWWRITEMAX);

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

/** Helper method to set the attributes of a serial connection.
 *  @param connection
 *    the serial port to connect to
 */
static int setSerAttr(struct serial_t *connection) {
  struct termios tty;
  memset(&tty, 0, sizeof(struct termios));
  if (tcgetattr(connection->fd, &tty) != 0)
    return -1;
  cfsetospeed(&tty, connection->baudrate);
  cfsetispeed(&tty, connection->baudrate);
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; /* 8 bit chars */
  tty.c_iflag &= ~IGNBRK;                     /* disable break processing */
  tty.c_lflag = 0;                            /* no signalling chars, no echo, no canonical */
  tty.c_oflag = 0;                            /* no remapping, no delays */
  tty.c_cc[VMIN] = 0;                         /* read doesn't block */
  tty.c_cc[VTIME] = 5;                        /* 0.5 seconds read timeout */
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);     /* shut off xon/xoff ctrl */
  tty.c_cflag |= (CLOCAL | CREAD);            /* ignore model controls, enable reading */
  tty.c_cflag &= ~(PARENB | PARODD);          /* shut off parity */
  tty.c_cflag |= connection->parity ? 1 : 0;  /* optionally turn on parity */
  /* tty.c_cflag &= ~(CSTOPB | CRTSCTS);      ** no stop bits */
  tty.c_cflag &= ~CSTOPB;                     /* no stop bits */
  if (tcsetattr(connection->fd, TCSANOW, &tty) != 0)
    return -1;
  return 0;
}

/** Threadable method to update the readbuf of the serial communication,
 *  as well as the connection itself.
 *  @param connection_arg
 *    the serial struct defined opaquely for thread function usage
 *  @note
 *    the packets will be read in the following format:
 *    data\n
 *    however, the \n will be cut off
 */
static void *_serial_update(void *connection_arg) {
  struct serial_t *connection;
  int numAvailable;
  int totalBytes;
  char *start_index, *end_index;

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
        if ((connection->fd = open(connection->port, O_RDWR)) != -1) {
          if (setSerAttr(connection) == -1) {
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

    /* update buffer */
    if ((numAvailable = read(connection->fd, connection->readbuf, SWREADMAX)) > 0) {
      connection->readbuf[numAvailable] = '\0';
      if ((totalBytes = strlen(connection->buffer) + numAvailable) >= SWBUFMAX) {
        totalBytes -= SWBUFMAX - 1;
        memmove(connection->buffer, &connection->buffer[totalBytes],
            (SWBUFMAX - totalBytes) * sizeof(char));
        connection->buffer[SWBUFMAX - totalBytes] = '\0';
      }
      strcat(connection->buffer, connection->readbuf);

      /* extract last data packet */
      if ((end_index = strrchr(connection->buffer, '\n')) != (char *)-1) {
        end_index[0] = '\0';
        start_index = strrchr(connection->buffer, '\n');
        if (start_index == (char *)-1)
          start_index = connection->buffer;
        else
          start_index++;
        totalBytes = (int)(end_index - start_index) + 1; /* include the delimeter */
        memcpy(connection->readbuf, start_index,
            totalBytes * sizeof(char));
        memmove(connection->buffer, &end_index[1],
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
 *  @note
 *    this will return a malloc'd string! be sure to free when done
 */
char *serial_read(struct serial_t *connection) {
  char *buf;
  if (connection->readAvailable) {
    buf = (char *)malloc((strlen(connection->readbuf) + 1) * sizeof(char));
    memcpy(buf, connection->readbuf, (strlen(connection->readbuf) + 1) * sizeof(char));
    connection->readAvailable = 0;
  }
  return buf;
}

/** Write a message to the serial communication link.
 *  @param connection
 *    the serial communication link to write to
 *  @param message
 *    the message to send over to the other side
 */
void serial_write(struct serial_t *connection, char *message) {
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
  if (connection->fd != -1)
    close(connection->fd);
  if (connection->port != NULL)
    free(connection->port);
  memset(connection, 0, sizeof(struct serial_t));
  connection->fd = -1;
}
