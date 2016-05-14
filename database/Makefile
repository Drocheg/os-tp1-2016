HEADERS_COMMON = ../common/config.h ../common/lib.h ../common/product.h ../common/order.h ../logging/logging.h ../common/comm.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/product.o ../common/order.o ../logging/logging.o ../common/commWithFIFOs.o

HEADERS_DATABASE = SQLite/sqlite3.h database.h dbConn.h
OBJECTS_DATABASE = SQLite/sqlite3.o databaseServer.o dbConn.o

HEADERS_COMMANDLINE =
OBJECTS_COMMANDLINE = dbCommandLine.o

HEADERS_TEST =
OBJECTS_TEST = test.o

CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../common -I SQLite -I ../logging

all: database

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_DATABASE)
	-rm -f $(OBJECTS_COMMANDLINE)

default: server

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

database: $(OBJECTS_COMMON) $(OBJECTS_DATABASE)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DATABASE) -o databaseServer.bin -lrt -lpthread -ldl

commandline: $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_COMMANDLINE)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_COMMANDLINE) -o dbCommandline.bin -lrt -lpthread -ldl
	
test:
	$(CC) $(CFLAGS) dbConn.c test.c -o test.bin -lrt -lpthread -ldl

.PHONY: all clean default database commandline test