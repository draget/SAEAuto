CC=g++
CFLAGS=-I. -I./xsens -c -Wall #-g -O0
LDFLAGS=-lrt -lboost_thread-mt -lncurses -lpthread -lboost_system -lgps
SOURCES=Control.cpp  CarNetwork.cpp Logger.cpp AsyncSerial.cpp SafetySerialOut.cpp LowLevelSerialOut.cpp GPSConnection.cpp IBEO.cpp IBEONetwork.cpp IPC.cpp PID.cpp  Xsens.cpp
DEPS = cmt1.h 
OBJSTATIC = ./xsens/cmt1.o ./xsens/cmt2.o ./xsens/cmt3.o ./xsens/cmtmessage.o ./xsens/cmtpacket.o ./xsens/cmtscan.o ./xsens/xsens_std.o ./xsens/xsens_time.o ./xsens/xsens_exception.o
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Control

all: static $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJSTATIC) $(OBJECTS) 
	$(CC) $(OBJECTS) $(OBJSTATIC) -o $@  $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 

static: $(OBJSTATIC)
	ar rcs libcmt.a $^


clean: 
	rm -f *.o
	rm -f ./xsens/*.o
	rm -f Control
	rm -f ./xsens/libcmt.a
	rm -f ./xsens/libcmt.so
