CC=g++
CFLAGS=-I. -Wall
LFLAGS=-lrt -lpthread
DEPS = cmt1.h 
OBJSTATIC = cmt1.o cmt2.o cmt3.o cmtmessage.o cmtpacket.o cmtscan.o xsens_std.o xsens_time.o xsens_exception.o

all: static example sae_recorder example1s example2s example3s

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

example: $(OBJSTATIC) example_linux.cpp
	$(CC) $(CFLAGS)  $(OBJSTATIC)  example_linux.cpp -lncurses $(LFLAGS) -o example

sae_recorder: $(OBJSTATIC) sae_recorder.cpp
	$(CC) $(CFLAGS)  $(OBJSTATIC)  sae_recorder.cpp $(LFLAGS) -o sae_recorder

example1s: $(OBJSTATIC) example1s_mt_linux.cpp
	$(CC) $(CFLAGS)  $(OBJSTATIC) example1s_mt_linux.cpp $(LFLAGS) -o example1s

example2s: $(OBJSTATIC) example2s_mt_linux.cpp
	$(CC) $(CFLAGS)  $(OBJSTATIC) example2s_mt_linux.cpp $(LFLAGS) -o example2s

example3s: $(OBJSTATIC) example3s_mt_linux.cpp
	$(CC) $(CFLAGS)  $(OBJSTATIC) example3s_mt_linux.cpp $(LFLAGS) -o example3s


static: $(OBJSTATIC)
	ar rcs libcmt.a $^

clean:
	rm -f example
	rm -f example1s
	rm -f example2s
	rm -f example3s
	rm -f sae_recorder
	rm -f *.o
	rm -f libcmt.a
	rm -f libcmt.so
