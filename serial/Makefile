CC = gcc
CCFLAGS = -Wall -pedantic -g -pthread

all: serial.o

serial.o: serial.c
	${CC} ${CCFLAGS} -c serial.c

clean:
	rm -rf *o
