HEADERS_COMMON = ../common/comm.h ../common/config.h ../common/lib.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/commWithFIFOs.o

HEADERS_SERVER = ../SQLite/src/sqlite3.h database.h server.h
OBJECTS_SERVER = ../SQLite/src/sqlite3.o database.o serverWithFIFOs.o


CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../common

default: all

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

server: $(OBJECTS_COMMON) $(OBJECTS_SERVER)
	$(CC) $(CFLAGS) -pthread -lpthread $(OBJECTS_COMMON) $(OBJECTS_SERVER) -o server.bin -ldl

all: server

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_SERVER)