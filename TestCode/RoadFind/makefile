CC=g++
CFLAGS=-I.  `pkg-config --cflags opencv` -c -Wall #-g -O0
LDFLAGS= `pkg-config --libs opencv`
SOURCES=findroad4.cpp
DEPS = cmt1.h 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=findroad

all:  $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 




