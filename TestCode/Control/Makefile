CC=g++
CFLAGS=-c -Wall #-g -O0
LDFLAGS=-lboost_thread-mt -lncurses -lpthread -lboost_system -lgps
SOURCES=Control.cpp  CarNetwork.cpp Logger.cpp AsyncSerial.cpp SafetySerialOut.cpp LowLevelSerialOut.cpp GPSConnection.cpp IBEO.cpp IBEONetwork.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Control

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 
