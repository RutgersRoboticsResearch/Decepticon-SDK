CC = gcc
CXX = g++
CCFLAGS = -std=c99 -g -Wall -pedantic -Werror -pthread
CXXFLAGS = -fPIC -g -Wall -pedantic -Werror `pkg-config --cflags opencv`
DEPS = serial.o decepticon.o

all: $(DEPS)

serial.o: serial.c
	$(CC) $(CCFLAGS) -c $< -o $@

decepticon.o: decepticon.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o
