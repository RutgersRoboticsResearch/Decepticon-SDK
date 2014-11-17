CFLAGS=-fPIC -g -Wall `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
INCLUDE = 
FREE_LIBS = -L/usr/local/lib -pthread
TARGET = test

all: $(TARGET)

$(TARGET): $(TARGET).o serial.o decepticon.o
	$(CXX) $(INCLUDE) $(CFLAGS) $^ -o $(TARGET) $(LIBS) $(FREE_LIBS)

$(TARGET).o: $(TARGET).cpp
	$(CXX) -c $(CFLAGS) $(TARGET).cpp -o $(TARGET).o $(INCLUDE)

serial.o: serial.c
	gcc -Wall -Werror -std=c99 -pedantic -pthread -c serial.c

decepticon.o: decepticon.cpp
	g++ -c $(CFLAGS) -c decepticon.cpp -o decepticon.o $(INCLUDE)

clean:
	rm -rf *.o $(TARGET)
