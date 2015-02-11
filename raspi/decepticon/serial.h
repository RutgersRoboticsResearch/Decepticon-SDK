#ifndef serial_h
#define serial_h

#include <stdint.h>
#define SWBUFMAX    64
#define SWREADMAX   32
#define SWWRITEMAX  32

#ifdef __cplusplus
extern "C" {
#endif

typedef struct serial {
  char    *port;
  int     fd;
  int8_t  connected;
  int     baudrate;
  int     parity;
 
  /* values */
  char    buffer[SWBUFMAX];
  char    readbuf[SWREADMAX];
  int8_t  readAvailable;
} serial_t;

int serial_connect(serial_t *connection, char *port, int baudrate, int parity = 0);
void serial_sync(serial_t *connection);
void serial_update(serial_t *connection);
char *serial_read(serial_t *connection);
void serial_write(serial_t *connection, char *message);
void serial_disconnect(serial_t *connection);

#ifdef __cplusplus
}
#endif

#endif
