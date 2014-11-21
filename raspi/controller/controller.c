#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <linux/joystick.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "controller.h"

#define INPUT_DIR "/dev/input/"
#define JS_PREFIX "js"
#define MAX_16BIT 0x7FFF
static void *_controller_update(void *controller_arg);

/** Connect to a joystick device.
 *  @param controller
 *    A pointer to a controller struct.
 */
void controller_connect(struct controller_t *controller) {
  DIR *dp;
  struct dirent *ent;

  /* find the device */
  if (!(dp = opendir(INPUT_DIR)))
    return;
  controller->name = NULL;
  controller->connected = 0;
  while ((ent = readdir(dp)))
    if (strstr(ent->d_name, JS_PREFIX)) {
      controller->name = (char *)malloc((strlen(INPUT_DIR) + strlen(ent->d_name) + 1) * sizeof(char));
      sprintf(controller->name, "%s%s", INPUT_DIR, ent->d_name);
      controller->fd = open(controller->name, O_RDONLY);
      if (controller->fd == -1)
        goto error;
      controller->connected = 1;
      controller->buttons = 0;
      controller->axes = 0;
      memset(&controller->A, 0, (size_t)controller + sizeof(struct controller_t) - (size_t)&controller->A);
      break;
    }
  if (!controller->connected)
    goto error;

  /* start asynchronous update */
  if (pthread_create(&controller->thread, NULL, _controller_update, (void *)controller) != 0)
    goto error;
  controller->alive = 1;
  return;

error:
  fprintf(stderr, "Error: Cannot connect to controller\n");
  controller->connected = 0;
  controller->alive = 0;
  if (controller->fd != -1)
    close(controller->fd);
  controller->fd = -1;
  if (controller->name != NULL)
    free(controller->name);
  controller->name = NULL;
  memset(controller, 0, (size_t)&controller->A - (size_t)controller);
}

/** Hidden. Update a joystick device asynchronously.
 *  @param controller_arg
 *    A (void *) pointer to a controller struct.
 *  @return NULL
 */
static void *_controller_update(void *controller_arg) {
  struct controller_t *controller;
  struct js_event event;

  controller = (struct controller_t *)controller_arg;
  while (controller->alive) {
    /* dynamically reconnect the device */
    if (access(controller->name, O_RDONLY) != 0) {
      if (controller->connected) {
        controller->connected = 0;
        controller->fd = -1;
        controller->buttons = 0;
        controller->axes = 0;
        memset(&controller->A, 0, (size_t)&controller->HOME + sizeof(char) - (size_t)&controller->A);
      }
    } else {
      if (!controller->connected) {
        controller->fd = open(controller->name, O_RDONLY);
        if (controller->fd != -1)
          controller->connected = 1;
      }
    }
    if (!controller->connected)
      continue;

    /* update device values */
    if (read(controller->fd, &event, sizeof(struct js_event)) == -1)
      continue;
    if (event.type & JS_EVENT_BUTTON) {
      if (event.type & JS_EVENT_INIT)
        controller->buttons++;
      switch (event.number) {
        case 0:
          controller->A = event.value;
          break;
        case 1:
          controller->B = event.value;
          break;
        case 2:
          controller->X = event.value;
          break;
        case 3:
          controller->Y = event.value;
          break;
        case 4:
          controller->LB = event.value;
          break;
        case 5:
          controller->RB = event.value;
          break;
        case 6:
          controller->START = event.value;
          break;
        case 7:
          controller->SELECT = event.value;
          break;
        case 8:
          controller->HOME = event.value;
          break;
        case 9:
          controller->LJOY.pressed = event.value;
          break;
        case 10:
          controller->RJOY.pressed = event.value;
          break;
      }
    } else if (event.type & JS_EVENT_AXIS) {
      float value;
      value = (float)event.value;
      if (value > 1.0 || value < -1.0)
        value /= (float)MAX_16BIT;
      if (event.type & JS_EVENT_INIT)
        controller->axes++;
      switch (event.number) {
        case 0:
          controller->LJOY.x = value;
          break;
        case 1:
          controller->LJOY.y = -value;
          break;
        case 2:
          controller->LT = value;
          break;
        case 3:
          controller->RJOY.x = value;
          break;
        case 4:
          controller->RJOY.y = -value;
          break;
        case 5:
          controller->RT = value;
          break;
        case 6:
          controller->LEFT = value < 0.0;
          controller->RIGHT = value > 0.0;
          break;
        case 7:
          controller->UP = value < 0.0;
          controller->DOWN = value > 0.0;
          break;
      }
    }
  }
  pthread_exit(NULL);
  return NULL;
}

/** Disconnect from a joystick device.
 *  @param controller
 *    A pointer to a controller struct.
 */
void controller_disconnect(struct controller_t *controller) {
  /* stop asynchronous update */
  if (controller->alive) {
    controller->alive = 0;
    pthread_join(controller->thread, NULL);
  }
  
  /* clean up */
  if (controller->fd != -1)
    close(controller->fd);
  if (controller->name != NULL)
    free(controller->name);
  memset(controller, 0, sizeof(struct controller_t));
  controller->fd = -1;
}
