CC=gcc
CFLAGS=-c -Wall -std=gnu99
LDFLAGS=
SOURCES=edc.c sbp.c 
OBJECTS=$(SOURCES:.c=.o)
OUT=libpiksi.a
	
$(OUT): $(OBJECTS) 
	ar rcs $(OUT) $(OBJECTS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
