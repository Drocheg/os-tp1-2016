#include <commWithFIFOs.h>
#include <config.h>
#include "forkedServer.h"
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <lib.h>
#include <signal.h>

void sigint_handler(int signum);	// TODO

static int isParentServer;

int main(int argc, char **argv) {
	char *FIFOpath = getServerAddress();
	remove(FIFOpath);	//Remove it if it was already present (e.g. forcibly closed from a previous run)
	ConnectionParams connParams = conn_listen(FIFOpath);
	if(connParams == NULL) {
		fprintf(stderr, "Couldn't set up requests FIFO in main server. Aborting.\n");
		remove(FIFOpath);
		return -1;
	}

	isParentServer = 1;
	printf("This is main server #%i, listening to connections at %s\n", getpid(), FIFOpath);
	//Main server loop
	while(1) {
		printf("Listening for clients...");
		fflush(stdout);
		Connection c = conn_accept(connParams);
		if(c == NULL)	//TODO
		printf("client connected.\n");
		if (!fork()) {   //Child
			isParentServer = 0;
			//Don't need main server FIFO anymore
			close(connParams->connRequestsFD);
			forkedServer(c);
			break;
		}
		else {	//Parent
			printf("Listening for clients...");
			fflush(stdout);
			//TODO implement mechanism for cleanly stopping main server (i.e. SIGINT)
		}
	}
	return 0;
}

void forkedServer(Connection c) {
	printf("This is forked server #%i, ready for connection.\n", getpid());

	int done = 0;
	while(!done) {
		size_t msgLen = 0;
		char *msg = NULL;
		conn_receive(c, (void **)&msg, &msgLen);
		if(strcmp(msg, MESSAGE_CLOSE) == 0) {
			printf("Received %s, shutting down forked server #%i\n", MESSAGE_CLOSE, getpid());
			fflush(stdout);
			// Free up resources, but don't use conn_close; the client called that function and we shouldn't use their pipes.
			close(c->inFD);
			close(c->outFD);
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

void sigint_handler(int signum)
{
	// TODO implement with signal() or sigaction()
	// if(isParentServer) {	//Main server shutting down, TODO call WAITPID on all children before shutting down or send special signal to all of them?
	// 	printf("Shutting down main server #%i.\n", getpid());
	// 	close(connParams->connRequestsFD);
	// 	remove(getServerAddress());
	// }
}