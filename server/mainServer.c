#include "comm.h"
#include "config.h"
#include <sys/types.h>
//#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
//#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

static int createForkedServer(Connection c) {
	int pid = fork();
	if (pid < 0) {
		fprintf(stderr, "Couldn't create a forked server\n");
		return -1;
	} else if (pid > 0) {
		/* Parent Process */
		return 0;
	} else {
		/* Child Process */
		return forkedServer(c, 0); //TODO El 0 es para hacer algo cabeza para testing. Hay que ver como hacemos la connection con base de datos
	}
}


int main(int argc, char *argv[]) {

	//int isParentProcess = 1;
	char *address = getServerAddress();
	/*remove(address); //Remove it if it was already present (e.g. forcibly closed from a previous run)*/
	ConnectionParams connParams = conn_listen(address);
	if (connParams == NULL) {
		fprintf(stderr, "Couldn't start server. Aborting.\n");
		return -1;
	}
	printf("Server started. Listening to connnection requests.\n");
	while(1) {
		Connection c = conn_accept(connParams);
		if (c == NULL) {
			fprintf(stderr, "Error accepting connection\n");
			exit(1);
		} else {
			int pid = createForkedServer(c);

		}
	}


}













