#ifndef __serial_h__
#define __serial_h__

#include <stdint.h>
#include <pthread.h>
#define SWBUFMAX    256
#define SWREADMAX   128
#define SWWRITEMAX  128

#ifdef __cplusplus
extern "C" {
#endif

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
  char    buffer[SWBUFMAX];
  char    readbuf[SWREADMAX];
  int8_t  readAvailable;
};

int serial_connect(struct serial_t *connection, char *port, int baudrate, int parity);
char *serial_read(struct serial_t *connection);
void serial_write(struct serial_t *connection, char *message);
void serial_disconnect(struct serial_t *connection);

#ifdef __cplusplus
}
#endif

#endif
