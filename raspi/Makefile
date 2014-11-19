CC = gcc
CXX = g++
CCFLAGS = -std=c99 -g -Wall -pedantic -Werror -pthread
CXXFLAGS = -fPIC -g -Wall -pedantic -Werror -pthread `pkg-config --cflags opencv`
LIBS = -L/usr/local/lib `pkg-config --libs opencv`

# this is your program
TARGET = agent

# these are your dependencies
DEPS = controller.o

all: $(DEPS) $(TARGET)

$(TARGET): $(TARGET).o $(DEPS)
	make -C decepticon
	$(CXX) $(CXXFLAGS) $^ decepticon/*.o -o $(TARGET) $(LIBS)

# customize your dependecy compilation here
controller.o:
	make -C controller
	cp controller/controller.o .

clean:
	make clean -C controller
	make clean -C decepticon
	rm -f *.o $(TARGET)
