CC = gcc
CCFLAGS = -Wall -pedantic -g -pthread

all: serial.o test.o test

test: test.o serial.o
	${CC} ${CCFLAGS} -o test test.o serial.o

test.o: test.c
	${CC} ${CCFLAGS} -c test.c

serial.o: serial.c
	${CC} ${CCFLAGS} -c serial.c

clean:
	rm -rf *o test
