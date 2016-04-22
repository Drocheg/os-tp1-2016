#include "lib.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Gets the position of <i>needle</i> in <i>haystack</i>, if found.
 * 
 * @see http://stackoverflow.com/questions/2091825/strpos-in-c-how-does-it-work
 * @param haystack The string to search in.
 * @param needle The string to search for.
 * @return The starting position of <i>needle</i> in <i>haystack</i>, or -1 if
 * not found.
 */
int indexOf(const char *haystack, const char *needle) {
    char *p = strstr(haystack, needle);
    if (p) {
        return p - haystack;        
    }
    return -1;
}

int ensureRead(void *dest, size_t bytes, int fd) {
	int readBytes = 0, readResult;
	while(readBytes < bytes) {
		readResult = read(fd, (char *)dest+readBytes, bytes-readBytes);
		if(readResult == -1) {
			return 0;
		}
		readBytes += readResult;
	}
	return 1;
}

int ensureWrite(const void *src, size_t bytes, int fd) {
	int writtenBytes = 0, writeResult;
	while(writtenBytes < bytes) {
		writeResult = write(fd, (char*)src+writtenBytes, bytes-writtenBytes);
		if(writeResult == -1) {
			return -1;
		}
		writtenBytes += writeResult;
	}
	return 1;
}
