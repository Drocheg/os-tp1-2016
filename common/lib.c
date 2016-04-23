#include "lib.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int ensureRead(void *dest, size_t bytes, int fd) {
	if(bytes == 0)
		return 1;

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
	if(bytes == 0)
		return 1;

	int writtenBytes = 0, writeResult;
	while(writtenBytes < bytes) {
		writeResult = write(fd, (char*)src+writtenBytes, bytes-writtenBytes);
		if(writeResult == -1) {
			return 0;
		}
		writtenBytes += writeResult;
	}
	return 1;
}
