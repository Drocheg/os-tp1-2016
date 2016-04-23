#include <commWithFIFOs.h>
#include <config.h>
#include "forkedServer.h"
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <lib.h>

static int attendConnection(Connection c, const int mainServerFD);

int main(int argc, char **argv) {
	//Create main server FIFO
	char *FIFOpath = getServerAddress();
	remove(FIFOpath);	//Remove it if it was already present (e.g. forcibly closed from a previous run)
	if(mkfifo(FIFOpath, 0666)) {	//-1 = error
		if(remove(FIFOpath) || mkfifo(FIFOpath, 0666)) {
			fprintf(stderr, "Error creating main server FIFO.");
			return -1;
		}
	}
	//Open it
	int mainServerFD = open(FIFOpath, O_RDWR|O_NONBLOCK);
	if(mainServerFD == -1) {
		fprintf(stderr, "Couldn't open main server FIFO. Aborting.\n");
		return -1;
	}
	//FIFO was opened in non-blocking read mode (otherwise server gets stuck) but now
	//we want it to be blocking. Remove nonblock flag.
	fcntl(mainServerFD, F_SETFL, fcntl(mainServerFD, F_GETFL)&~O_NONBLOCK);

	int isParentServer = 1;
	int done = 0;
	fd_set rfds;		//File Descriptor set for select() - has to be reconstructed every time
	struct timeval tv;	//Timeout struct for select()

	printf("This is main server #%i, listening to connections at %s\n", getpid(), FIFOpath);
	printf("Listening for clients...");
	fflush(stdout);
	//Main server loop
	while(isParentServer || !done) {
		FD_ZERO(&rfds);
		FD_SET(mainServerFD, &rfds);
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		int selectResult = select(mainServerFD+1, &rfds, NULL, NULL, &tv);
		if(selectResult == 0) {	//Timed out
			printf(".");
			fflush(stdout);
		}
		else if (selectResult == -1) {	//Error
			printf("Error with select(). Aborting.\n");
			return -1;
		}
		else {
			printf("client connected.\n");
			Connection c = malloc(sizeof(*c));
			if(!attendConnection(c, mainServerFD)) {
				printf("Error, couldn't read connection request. Ignoring?\n");
			}
			else {
				//Basic connection setup complete
				if (!fork()) {   //Child
					isParentServer = 0;
					//Don't need main server FIFO anymore
					close(mainServerFD);
					forkedServer(c);
				}
				else {	//Parent
					printf("Listening for clients...");
					fflush(stdout);
				}
			}
		}
	}
	if(isParentServer) {	//Main server shutting down, call WAITPID on all children before shutting down or send special signal to all of them?
		printf("Shutting down main server #%i.\n", getpid());
		close(mainServerFD);
		remove(getServerAddress());
	}
	return 0;
}

/**
 * Reads basic connection information from the main server FIFO
 * (only parent server should do this), forked server will complete
 * connection. Specifically, this is step 4 in the connection process.
 *
 * @param Connection c The connection to load information into.
 * @param int mainServerFD The file descriptor of the main server FIFO.
 * @return int 1 On success, 0 on error. On error, <i>c</i> might have
 * incomplete or invalid information.
 * @see conn_open() in commWithFIFOs.c
 */
static int attendConnection(Connection c, const int mainServerFD) {
	//Read out pipe (in for server)
	size_t lenIn = -1;
	if(!ensureRead(&lenIn, sizeof(lenIn), mainServerFD))
		return 0;
	c->inFIFOPath = malloc(lenIn);
	if(!ensureRead(c->inFIFOPath, lenIn, mainServerFD))
		return 0;
	//	printf("Read %i bytes for in pipe: %s\n", lenIn, c->inFIFOPath);
	//Read in pipe (out for server)
	size_t lenOut = -1;
	if(!ensureRead(&lenOut, sizeof(lenOut), mainServerFD))
		return 0;
	c->outFIFOPath = malloc(lenOut);
	if(!ensureRead(c->outFIFOPath, lenOut, mainServerFD))
		return 0;
	//	printf("Read %i bytes for out pipe: %s\n", lenOut, c->outFIFOPath);
	return 1;
}

void forkedServer(Connection c) {
	printf("This is forked server #%i, completing connection\n", getpid());

	//Complete connection started by parent - this is not done in parent to avoid having all the childrens' FDs
	int inFD = open(c->inFIFOPath, O_RDONLY|O_NONBLOCK);
	fcntl(inFD, F_SETFL, fcntl(inFD, F_GETFL)&~O_NONBLOCK);	//Remove nonblocking flag
	c->inFD = inFD;
	int outFD = open(c->outFIFOPath, O_WRONLY);	//This should not fail, client has opened in read mode
	c->outFD = outFD;
	conn_send(c, MESSAGE_OK, strlen(MESSAGE_OK)+1);//Ready, send ACK

	//Connection complete, start listening to requests
	printf("Connection established, listening to requests...\n");
	int done = 0;
	while(!done) {
		size_t msgLen = 0;
		char *msg = NULL;
		conn_receive(c, (void **)&msg, &msgLen);
		if(strcmp(msg, MESSAGE_CLOSE) == 0) {
			printf("Received %s, shutting down forked server #%i\n", MESSAGE_CLOSE, getpid());
			fflush(stdout);
			//Don't use conn_close, the client called that function and we shouldn't use their pipes.
			//Free up resources
			close(inFD);
			close(outFD);
			free(c->inFIFOPath);
			free(c->outFIFOPath);
			free(c);
			done = 1;
		}
		else {
			printf("Received %s\n", msg);
		}
		if(msg != NULL) {
			free(msg);
		}
	}
}
