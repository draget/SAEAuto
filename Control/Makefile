CXX=g++
CXXFLAGS=-I. -I./xsens `pkg-config --cflags opencv` -c -Wall -pg #-g -O0
CC=gcc
CFLAGS=-I. -I./Piksi -c -Wall -std=gnu99 
LDFLAGS=-lrt -pg -lboost_thread -lncurses -lpthread -lboost_system -lgps -Lmatlab -lmatlab `pkg-config --libs opencv` -L./Piksi -lsbp-static
SOURCES=Control.cpp  CarNetwork.cpp Logger.cpp AsyncSerial.cpp SafetySerialOut.cpp LowLevelSerialOut.cpp GPSConnection.cpp IBEO.cpp IBEONetwork.cpp IPC.cpp PID.cpp  Xsens.cpp Fusion.cpp BoeingKalman.cpp Piksi_Read.c
DEPS = cmt1.h 
OBJSTATIC = ./xsens/cmt1.o ./xsens/cmt2.o ./xsens/cmt3.o ./xsens/cmtmessage.o ./xsens/cmtpacket.o ./xsens/cmtscan.o ./xsens/xsens_std.o ./xsens/xsens_time.o ./xsens/xsens_exception.o
OBJECTS=$(SOURCES:.cpp=.o)
COBJECT=$(SOURCES:.c=.o)
EXECUTABLE=Control

all: static $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJSTATIC) $(OBJECTS) $(COBJECTS)
	$(CC) $(OBJECTS) $(COBJECTS) $(OBJSTATIC) -o $@  $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@ 
.c.o:
	$(CC) $(CFLAGS) $< -o $@

static: $(OBJSTATIC)
	ar rcs libcmt.a $^


clean: 
	rm -f *.o
	rm -f ./xsens/*.o
	rm -f Control
	rm -f ./xsens/libcmt.a
	rm -f ./xsens/libcmt.so

