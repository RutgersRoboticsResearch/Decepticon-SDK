#include "serial.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void printasint(char *c) {
}

int main(int argc, char **argv){
    if(argc != 2){
        printf("Need an argument to specify port\n");
        return 1;
    }
    char *port = NULL;
    struct serial_t connection;
    printf("connecting...\n");
    serial_connect(&connection, port, 115200, 0);
    printf("connected!\n");
    for (;;) {
        char* test = serial_read(&connection);
        if (test) {
          printf("read: %s\n", test);
          free(test);
        }
    }
    serial_disconnect(&connection);
    return 0;
}
