HEADERS_LOGGING = logging.h
OBJECTS_LOGGING = logging.o

HEADERS_DAEMON = logging.h
OBJECTS_DAEMON = loggingDaemon.o

HEADERS_TESTER = logging.h
OBJECTS_TESTER = loggingTester.o

CC = gcc
CFLAGS = -g -Wall -std=c99

default: daemon

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

daemon: $(OBJECTS_LOGGING) $(OBJECTS_DAEMON)
	$(CC) $(CFLAGS) $(OBJECTS_LOGGING) $(OBJECTS_DAEMON) -o loggingDaemon.bin	

tester: $(OBJECTS_LOGGING) $(OBJECTS_TESTER)
	$(CC) $(CFLAGS) $(OBJECTS_LOGGING) $(OBJECTS_TESTER) -o loggingTester.bin		

all: daemon tester

clean:
	-rm -f $(OBJECTS_LOGGING)
	-rm -f $(OBJECTS_DAEMON)
	-rm -f $(OBJECTS_TESTER)
	
.PHONY: default all clean daemon tester