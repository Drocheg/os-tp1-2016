HEADERS_COMMON = ../common/config.h ../common/lib.h ../common/product.h ../common/order.h ../logging/logging.h ../database/dbConn.h ../common/comm.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/product.o ../common/order.o ../logging/logging.o ../database/dbConn.o ../common/commWithFIFOs.o

HEADERS_SERVER = forkedServer.h
OBJECTS_SERVER = mainServer.o forkedServer.o

CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../common -I ../database -I ../logging

all: server

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_SERVER)

default: server

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

server: $(OBJECTS_COMMON) $(OBJECTS_SERVER)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_SERVER) -o server.bin -lrt -lpthread -ldl

.PHONY: all clean default server