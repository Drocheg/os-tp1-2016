#include <stdio.h>
#include <sys/types.h>
#include "comm.h"
#include "aux.h"
#include <string.h>
#include <signal.h>

static int serverPID;

static void finish(int signal) {

	kill(serverPID, SIGINT);
}




int main(int argc, char *argv[]) {

	Connection connection = conn_open(argv[1]);
	char * rta;
	size_t length;

	conn_receive(connection, (void**)&rta, &length);
	conn_receive(connection, (void**)&serverPID, &length);
	printf("%s\n", rta);
	while(1) {

		char * aux = getLine();
		conn_send(connection, aux, strlen(aux) + 1);
		conn_receive(connection, (void**)&rta, &length);
		printf("%s\n", rta);
	}
}