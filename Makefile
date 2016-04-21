HEADERS_COMMON = common/comm.h common/config.h common/lib.h
OBJECTS_COMMON = common/config.o common/lib.o

HEADERS_SERVER = SQLite/src/sqlite3.h server/database.h
OBJECTS_SERVER = SQLite/src/sqlite3.o server/database.o

HEADERS_CLIENT = 
OBJECTS_CLIENT = client/clientWithFIFOs.o common/commWithFIFOs.o


CC = gcc
CFLAGS = -g -Wall -std=c99 -I common
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