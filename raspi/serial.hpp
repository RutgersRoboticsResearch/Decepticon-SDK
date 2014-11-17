#ifndef __serial_hpp__
#define __serial_hpp__

#include <stdint.h>
#include <pthread.h>
#define SWBUFMAX    512
#define SWREADMAX   256
#define SWWRITEMAX  256

struct serial_t {
  char    *port;
  int     fd;
  int8_t  connected;
  int     baudrate;
  int     parity;
 
  /* threaded update */
  pthread_t thread;
  int8_t    alive;

  /* values */
  char    *id;
  char    buffer[SWBUFMAX];
  char    readbuf[SWREADMAX];
  int8_t  readAvailable;
  char    writebuf[SWWRITEMAX];
  int8_t  writeAvailable;
};

extern "C" {
  int serial_connect(struct serial_t *connection, char *port, int baudrate, int parity);
  char *serial_read(struct serial_t *connection);
  void serial_write(struct serial_t *connection, char *message);
  void serial_disconnect(struct serial_t *connection);
}

#endif
