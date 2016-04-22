HEADERS_COMMON = ../common/comm.h ../common/config.h ../common/lib.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/commWithFIFOs.o

HEADERS_CLIENT = 
OBJECTS_CLIENT = clientWithFIFOs.o


CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../common

default: all

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

client: $(OBJECTS_COMMON) $(OBJECTS_CLIENT)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_CLIENT) -o client.bin	

all: client

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_CLIENT)