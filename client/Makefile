HEADERS_COMMON = ../common/comm.h ../common/config.h ../common/lib.h ../common/product.h  ../common/order.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/commWithFIFOs.o ../common/product.o ../common/order.o

HEADERS_CLIENT = 
OBJECTS_CLIENT = client.o

HEADERS_DUMMY_CLIENT = 
OBJECTS_DUMMY_CLIENT = dummyClient.o

CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../common

default: client

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

client: $(OBJECTS_COMMON) $(OBJECTS_CLIENT)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_CLIENT) -o client.bin
	
dummy: $(OBJECTS_COMMON) $(OBJECTS_DUMMY_CLIENT)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DUMMY_CLIENT) -o dummyClient.bin

all: client dummy

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_CLIENT)
	-rm -f $(OBJECTS_DUMMY_CLIENT)