CFLAGS=-fPIC -g -Wall `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
INCLUDE = ''
FREE_LIBS = -L/usr/local/lib -pthread
TARGET = test

all: $(TARGET)

#$(TARGET): $(TARGET).cpp
#	$(CXX) $(INCLUDE) $(CFLAGS) $? -o $@ $(LIBS) $(FREE_LIBS)

$(TARGET): $(TARGET).o 
	$(CXX) $(INCLUDE) $(CFLAGS) $(TARGET).o -o $(TARGET) $(LIBS) $(FREE_LIBS)

#%.o: %.cpp
#	$(CXX) -c $(CFLAGS) $< -o $@

$(TARGET).o: $(TARGET).cpp
	$(CXX) -c $(CFLAGS) $(TARGET).cpp -o $(TARGET).o $(INCLUDE)

clean:
	rm -rf *.o $(TARGET)
