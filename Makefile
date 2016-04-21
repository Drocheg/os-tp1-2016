HEADERS_COMMON = planning/comm.h planning/config.h planning/lib.h
OBJECTS_COMMON = planning/config.o planning/lib.o

HEADERS_SERVER = SQLite/src/sqlite3.h planning/database.h
OBJECTS_SERVER = SQLite/src/sqlite3.o planning/database.o

HEADERS_CLIENT = 
OBJECTS_CLIENT = planning/client.o planning/commWithFIFOs.o


CC = gcc
CFLAGS = -g -Wall -std=c99
#-pthread option for multithreading

default: client

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

client: $(OBJECTS_COMMON) $(OBJECTS_CLIENT)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_CLIENT) -o client

server: $(OBJECTS_COMMON) $(OBJECTS_SERVER)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_SERVER) -o $server
	$(LD) -o server.exe $(OBJECTS_COMMON) $(OBJECTS_CLIENT)

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_CLIENT)
	-rm -f $(OBJECTS_SERVER)