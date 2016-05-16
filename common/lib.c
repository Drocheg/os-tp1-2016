#include "lib.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>

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

int select_wrapper(int maxFD, const int readFDs[], int rCount, const int writeFDs[], int wCount, const int errFDs[], int eCount, int timeoutSec, int timeoutUSec) {
    fd_set readSet, writeSet, errSet;
    FD_ZERO(&readSet);
    if(readFDs != NULL) {
        for(int i = 0; i < rCount; i++) {
            FD_SET(readFDs[i], &readSet);
        }
    }
    FD_ZERO(&writeSet);
    if(writeFDs != NULL) {
        for(int i = 0; i < wCount; i++) {
            FD_SET(writeFDs[i], &writeSet);
        }
    }
    FD_ZERO(&errSet);
    if(errFDs != NULL) {
        for(int i = 0; i < eCount; i++) {
            FD_SET(errFDs[i], &errSet);
        }
    }
    struct timeval timeout;
    timeout.tv_sec = timeoutSec < 0 ? 0 : timeoutSec;
    timeout.tv_usec = timeoutUSec < 0 ? 0 : timeoutUSec;
    return select(maxFD,
                    readFDs == NULL ? NULL : &readSet,
                    writeFDs == NULL ? NULL : &writeSet,
                    errFDs == NULL ? NULL : &errSet,
                    timeoutSec < 0 && timeoutUSec < 0 ? NULL : &timeout);
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
        printf("%s", msg);
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