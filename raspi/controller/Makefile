CC = gcc
CCFLAGS = -std=c99 -g -Wall -pedantic -Werror -pthread
TARGET = controller

all: $(TARGET).o

$(TARGET).o: $(TARGET).c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f *.o
