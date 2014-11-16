#include "serial.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv){
    if(argc != 2){
        printf("Need an argument to specify port\n");
        return 1;
    }
    char *port = argv[1];
    struct serial_t connection;
    serial_connect(&connection, port, 115200, 0);
    while(1){
        char* test = serial_read(&connection);
        printf("%s\n", test);
    }
    serial_disconnect(&connection);
    return 0;
}
