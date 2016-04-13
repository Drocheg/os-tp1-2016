#include "comm.h"
#include <stdlib.h>
#include <signal.h>

#define String char *
#define CONF_FILE ~/some/path


struct connection_t {

    int inPipe[2]; /* Pipe for sending data into the server */
	int outPipe [2]; /* Pipe for sending data out of the server */
    int clientPID;
    int serverPID;
};

Connection openConnection(Server server) {

	Connection connection = malloc(sizeof(*connection));
	int inPipe = pipe(connection->inPipe), outPipe = pipe(connection->outPipe);
	if (inPipe != 0 || outPipe != 0) {
		exit(1); /* pipe creation wasn't completed */
	}

}