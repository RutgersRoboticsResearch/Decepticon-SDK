CC = gcc
CXX = g++
CCFLAGS = -std=c99 -g -Wall -pedantic -Werror -pthread
CXXFLAGS = -fPIC -g -Wall -pedantic -Werror `pkg-config --cflags opencv`
DEPS = serial.o decepticon.o
INCLUDE = -I$(HOME)/raspi/decepticon/robidouille/raspicam_cv

all: $(DEPS)

serial.o: serial.c
	$(CC) $(CCFLAGS) -c $< -o $@

decepticon.o: decepticon.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -f *.o
