HEADERS_COMMON = ../common/config.h ../common/lib.h ../common/product.h ../common/order.h ../logging/logging.h ../common/comm.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/product.o ../common/order.o ../logging/logging.o 

OBJECTS_SOCKETS = ../common/commWithSockets.o
OBJECTS_FIFOS = ../common/commWithFIFOs.o

HEADERS_SERVER = forkedServer.h dbConn.h
OBJECTS_SERVER = mainServer.o forkedServer.o dbConn.o

CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../common -I ../logging

all: sockets

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_SERVER)
	-rm -f $(OBJECTS_FIFOS)
	-rm -f $(OBJECTS_SOCKETS)

default: sockets

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#server: $(OBJECTS_COMMON) $(OBJECTS_SERVER)
#	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_SERVER) -o server.bin -lrt -lpthread -ldl

fifos: $(OBJECTS_COMMON) $(OBJECTS_SERVER) $(OBJECTS_FIFOS)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_SERVER) $(OBJECTS_FIFOS) -o server.bin -lrt -lpthread -ldl

sockets: $(OBJECTS_COMMON) $(OBJECTS_SERVER) $(OBJECTS_SOCKETS)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_SERVER) $(OBJECTS_SOCKETS) -o server.bin -lrt -lpthread -ldl


.PHONY: all clean default server fifos sockets
