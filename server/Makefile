HEADERS_COMMON = ../common/comm.h ../common/config.h ../common/lib.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/commWithFIFOs.o

HEADERS_SERVER = server.h
OBJECTS_SERVER = serverWithFIFOs.o

HEADERS_DATABASE = ../SQLite/src/sqlite3.h database.h
OBJECTS_DATABASE = ../SQLite/src/sqlite3.o database.o

HEADERS_DATABASE_COMMANDLINE =
OBJECTS_DATABASE_COMMANDLINE = databaseCommandLine.o

CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../commoner.bin
	
database: #$(OBJECTS_DATABASE)
	#$(CC) $(CFLAGS) -pthread -lpthread $(OBJECTS_DATABASE) -o database.bin -ldl
	
database_commandline: $(OBJECTS_DATABASE) $(OBJECTS_DATABASE_COMMANDLINE)
	$(CC) $(CFLAGS) -pthread -lpthread $(OBJECTS_DATABASE) $(OBJECTS_DATABASE_COMMANDLINE) -o db_commandline.bin -ldl

all: server database

clean:
	-rm -f $(OBJECTS_COMMON)

default: server

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

server: $(OBJECTS_COMMON) $(OBJECTS_SERVER)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_SERVER) -o serv
	-rm -f $(OBJECTS_SERVER)
	-rm -f $(OBJECTS_DATABASE)