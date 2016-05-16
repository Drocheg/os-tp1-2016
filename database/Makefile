HEADERS_COMMON = ../common/config.h ../common/lib.h ../common/product.h ../common/order.h ../logging/logging.h ../common/comm.h
OBJECTS_COMMON = ../common/config.o ../common/lib.o ../common/product.o ../common/order.o ../logging/logging.o

OBJECTS_SOCKETS = ../common/commWithSockets.o
OBJECTS_FIFOS = ../common/commWithFIFOs.o

HEADERS_DATABASE = SQLite/sqlite3.h dbConn.h database.h
OBJECTS_DATABASE = SQLite/sqlite3.o dbConn.o

HEADERS_SERVER =
OBJECTS_SERVER = databaseServer.o

HEADERS_COMMANDLINE =
OBJECTS_COMMANDLINE = dbCommandLine.o

HEADERS_TEST =
OBJECTS_TEST = test.o

CC = gcc
CFLAGS = -g -Wall -std=c99 -I ../common -I SQLite -I ../logging

all: sockets

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_SERVER)
	-rm -f $(OBJECTS_COMMANDLINE)
	-rm -f $(OBJECTS_FIFOS)
	-rm -f $(OBJECTS_SOCKETS)
	-rm -f *.o

default: sockets

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#server: $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_SERVER)
#	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_SERVER) -o databaseServer.bin -lrt -lpthread -ldl

#commandline: $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_COMMANDLINE)
#	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_COMMANDLINE) -o dbCommandline.bin -lrt -lpthread -ldl


fifos: serverfifos commandlinefifos

sockets: serversockets commandlinesockets


serverfifos: $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_SERVER) $(OBJECTS_FIFOS)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_SERVER) $(OBJECTS_FIFOS) -o databaseServer.bin -lrt -lpthread -ldl

commandlinefifos: $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_COMMANDLINE) $(OBJECTS_FIFOS)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_COMMANDLINE) $(OBJECTS_FIFOS) -o dbCommandline.bin -lrt -lpthread -ldl

serversockets: $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_SERVER) $(OBJECTS_SOCKETS)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_SERVER) $(OBJECTS_SOCKETS) -o databaseServer.bin -lrt -lpthread -ldl

commandlinesockets: $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_COMMANDLINE) $(OBJECTS_SOCKETS)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_DATABASE) $(OBJECTS_COMMANDLINE) $(OBJECTS_SOCKETS) -o dbCommandline.bin -lrt -lpthread -ldl

	
test:
	$(CC) $(CFLAGS) dbConn.c test.c -o test.bin -lrt -lpthread -ldl

.PHONY: all clean default server commandline test sockets fifos
