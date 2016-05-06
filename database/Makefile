HEADERS_COMMON = ../common/config.h ../common/lib.h ../common/product.h ../common/order.h ../common/comm.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/product.o ../common/order.o ../common/commWithFIFOs.o

HEADERS_DATABASE = SQLite/sqlite3.h database.h
OBJECTS_DATABASE = SQLite/sqlite3.o database.o

HEADERS_COMMANDLINE =
OBJECTS_COMMANDLINE = dbCommandLine.o

CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../common -I SQLite

all: database

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_DATABASE)
	-rm -f $(OBJECTS_COMMANDLINE)

default: server

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

database: $(OBJECTS_COMMON) $(OBJECTS_DATABASE)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DATABASE) -o db_server.bin -pthread -lpthread -ldl

commandline: $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_COMMANDLINE)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_COMMANDLINE) -o db_commandline.bin -pthread -lpthread -ldl