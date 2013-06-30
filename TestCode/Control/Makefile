CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lboost_thread-mt -lncurses
SOURCES=Control.cpp  CarNetwork.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Control

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 
