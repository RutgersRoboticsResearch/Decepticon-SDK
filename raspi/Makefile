CC = gcc
CXX = g++
DECEPTICON_DIR = $(HOME)/raspi/decepticon
CCFLAGS = -std=c99 -g -Wall -pedantic -Werror -pthread
CXXFLAGS = -fPIC -g -Wall -pedantic -Werror -pthread `pkg-config --cflags opencv`
LIBS = -L/usr/local/lib `pkg-config --libs opencv`
RASPICAM_LIBS = -L$(DECEPTICON_DIR)/robidouille/raspicam_cv -lraspicamcv -L$(DECEPTICON_DIR)/userland/build/lib \
	-lmmal_core -lmmal -l mmal_util -lvcos -lbcm_host
INCLUDE = -I$(DECEPTICON_DIR)/robidouille/raspicam_cv

# R3: this is your program
TARGET = agent

# R3: these are your dependencies
DEPS = controller.o

all: $(DEPS) $(TARGET)

$(TARGET): $(TARGET).o $(DEPS)
	make -C decepticon
	$(CXX) $(INCLUDE) $(CXXFLAGS) $^ decepticon/*.o -o $(TARGET) $(LIBS) $(RASPICAM_LIBS)

$(TARGET).o: $(TARGET).cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

# R3: customize your dependecy compilation here
controller.o:
	make -C controller
	cp controller/controller.o .

clean:
	make clean -C controller
	make clean -C decepticon
	rm -f *.o $(TARGET)
