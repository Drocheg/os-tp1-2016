HEADERS_COMMON = ../common/comm.h ../common/config.h ../common/lib.h ../common/product.h  ../common/order.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/product.o ../common/order.o

OBJECTS_SOCKETS = ../common/commWithSockets.o
OBJECTS_FIFOS = ../common/commWithFIFOs.o

HEADERS_CLIENT = 
OBJECTS_CLIENT = client.o

HEADERS_DUMMY_CLIENT = 
OBJECTS_DUMMY_CLIENT = dummyClient.o

CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../common

default: sockets

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

#client: $(OBJECTS_COMMON) $(OBJECTS_CLIENT)
#	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_CLIENT) -o client.bin
	
dummy: $(OBJECTS_COMMON) $(OBJECTS_DUMMY_CLIENT)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DUMMY_CLIENT) $(OBJECTS_SOCKETS) -o dummyClient.bin


fifos: $(OBJECTS_COMMON) $(OBJECTS_CLIENT) $(OBJECTS_FIFOS)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_CLIENT) $(OBJECTS_FIFOS) -o client.bin


sockets: $(OBJECTS_COMMON) $(OBJECTS_CLIENT) $(OBJECTS_SOCKETS)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_CLIENT) $(OBJECTS_SOCKETS) -o client.bin



all: sockets dummy

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_CLIENT)
	-rm -f $(OBJECTS_DUMMY_CLIENT)
	-rm -f $(OBJECTS_FIFOS)
	-rm -f $(OBJECTS_SOCKETS)

.PHONY: default all clean client dummy fifos sockets
