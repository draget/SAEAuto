CXX=g++
CXXFLAGS=-I. -I./xsens `pkg-config --cflags opencv` -c -Wall -Wuninitialized -ggdb -g #-O0
CC=gcc
CFLAGS=-I. -c -Wall -g -std=gnu99
LDFLAGS=-lrt -lboost_thread -lncurses -lpthread -lboost_system -lgps -Lmatlab -lmatlab `pkg-config --libs opencv` -LPiksi -lpiksi -lstdc++ -lm
SOURCES=Control.cpp  CarNetwork.cpp Logger.cpp AsyncSerial.cpp SafetySerialOut.cpp LowLevelSerialOut.cpp GPSConnection.cpp IBEO.cpp IBEONetwork.cpp IPC.cpp PID.cpp  Xsens.cpp Fusion.cpp BoeingKalman.cpp Piksi_Read.c
DEPS = cmt1.h 
OBJSTATIC = ./xsens/cmt1.o ./xsens/cmt2.o ./xsens/cmt3.o ./xsens/cmtmessage.o ./xsens/cmtpacket.o ./xsens/cmtscan.o ./xsens/xsens_std.o ./xsens/xsens_time.o ./xsens/xsens_exception.o
OBJECTS=$(SOURCES:.cpp=.o)
COBJECTS=$(OBJECTS:.c=.o)
EXECUTABLE=Control

all: static $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJSTATIC) $(OBJECTS) $(COBJECTS)
	$(CC) $(COBJECTS) $(OBJSTATIC) -o $@  $(LDFLAGS)

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
#	rm -f ./Piksi/*.o
#	rm -f ./Piksi/*.so
