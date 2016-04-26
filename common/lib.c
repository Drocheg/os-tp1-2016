#include "lib.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

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

int countDigits(int number) {
    int i = (number <= 0) ? 1 : 0;
    
    while(number != 0) {
        number /= 10;
        i++;
    }
    return i;
}

int scanInt(const char* msg)
{
    int result, done = 0;
    do {
        printf(msg);
        if(scanf("%d",&result) != 1)
        {
            while(getchar() != '\n');   //Empty whatever's left in the buffer
            printf("Invalid input.\n");
        }
        else
            done = 1;
    } while (!done);
    while(getchar() != '\n');   //Empty whatever's left in the buffer
    return result;
}