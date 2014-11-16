CC=gcc
CFLAGS=-g -Wall -c
SOURCES=serial.c test.c
LINKER=-pthread
OBJECTS=$(SOURCES:.c=.o)
	EXECUTABLE=test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LINKER) -o $@

.c.o: 
	$(CC) $(CFLAGS) $< $(LINKER) -o $@

clean:
	rm -rf *o $(EXECUTABLE)
