HEADERS_COMMON = ../common/comm.h ../common/config.h ../common/lib.h ../common/product.h  ../common/order.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/commWithFIFOs.o ../common/product.o ../common/order.o

HEADERS_SERVER = forkedServer.h
OBJECTS_SERVER = mainServer.o forkedServer.o

HEADERS_DATABASE = ../SQLite/src/sqlite3.h database.h
OBJECTS_DATABASE = ../SQLite/src/sqlite3.o database.o

HEADERS_DATABASE_COMMANDLINE =
OBJECTS_DATABASE_COMMANDLINE = databaseCommandLine.o

CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../common

all: server

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_SERVER)

default: server

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

server: $(OBJECTS_COMMON) $(OBJECTS_SERVER)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_SERVER) -o server.bin