CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lboost_thread-mt -lncurses -lpthread -lboost_system
SOURCES=Control.cpp  CarNetwork.cpp Logger.cpp AsyncSerial.cpp SafetySerialOut.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Control

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 
