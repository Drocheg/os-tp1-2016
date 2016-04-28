HEADERS_COMMON = 
OBJECTS_COMMON =

HEADERS_LOGGER = logger.h 
OBJECTS_LOGGER = logger.o


CC = gcc
CFLAGS = -g -Wall -std=c99

default: all

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

logger: $(OBJECTS_COMMON) $(OBJECTS_LOGGER)
	$(CC) $(CFLAGS) $(OBJECTS_COMMON) $(OBJECTS_LOGGER) -o logger.bin	

all: logger

clean:
	-rm -f $(OBJECTS_COMMON)
	-rm -f $(OBJECTS_LOGGER)
