#include "comm.h"


struct connection_t
{
	int inPipe[2]; /* Pipe for sending data into the server */
	int outPipe [2]; /* Pipe for sending data out of the server */
	int clientPID;
	int serverPID;
};



Connection openConnection(Server server) {

	Connection connection = malloc(sizeof(*connection));
}