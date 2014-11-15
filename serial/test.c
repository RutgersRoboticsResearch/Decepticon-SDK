#include "serial.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static char exit_signal;

void interrupt(int signum) {
  exit_signal = 1;
}

int main(void) {
  struct serial_t connection;
  signal(SIGINT, interrupt);
  if (serial_connect(&connection, "/dev/ttyACM1", 115200, 0) == -1) {
    fprintf(stderr, "Cannot connect to device\n");
    return -1;
  }
  printf("Connected to device %s\n", connection.port);
  while (!exit_signal) {
    if (connection.readAvailable)
      printf("%s\n", connection.readbuf);
  }
  serial_disconnect(&connection);
  printf("Disconnected.\n");
  return 0;
}
