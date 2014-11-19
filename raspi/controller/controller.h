#ifndef __controller_h__
#define __controller_h__

#include <stdint.h>
#include <pthread.h>

/* Controller structure */
struct controller_t {
  char    *name;      /* name of the device */
  int32_t fd;         /* file descriptor of the device */
  int8_t  connected;  /* is the device connected */
  int32_t buttons;
  int32_t axes;

  /* threaded update */
  pthread_t thread;
  int8_t    alive;  /* 1 if alive, 0 if dead */

  /* values */
  int8_t  A;
  int8_t  B;
  int8_t  X;
  int8_t  Y;
  int8_t  UP;
  int8_t  DOWN;
  int8_t  LEFT;
  int8_t  RIGHT;
  int8_t  LB;
  int8_t  RB;
  float   LT;
  float   RT;
  struct {
    int8_t  pressed;
    float   x;
    float   y;
  } LJOY, RJOY;
  int8_t  START;
  int8_t  SELECT;
  int8_t  HOME;
};

/* Prototypes */
void controller_connect(struct controller_t *controller);
void controller_disconnect(struct controller_t *controller);

#endif
