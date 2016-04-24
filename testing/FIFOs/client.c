#include <stdio.h>
#include <sys/types.h>
#include "comm.h"
#include "aux.h"
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>


static pid_t serverPID;

static void finish(int signal) {

	kill(serverPID, SIGKILL);
	printf("Bye bye\n");
	exit(0);
}




int main(int argc, char *argv[]) {

	Connection connection = conn_open(argv[1]);
	pid_t * pid;
	char * rta;
	size_t length;

	conn_receive(connection, (void**)&rta, &length);
	conn_receive(connection, (void**)&pid, &length);
	serverPID = pid[0];
	free(pid);
	/*signal(SIGINT, finish);*/
	printf("%s\n", rta);
	printf("Server PID: %d\n", serverPID);
	while(1) {

		char * aux = getLine();
		conn_send(connection, aux, strlen(aux) + 1);
		free(aux);
		conn_receive(connection, (void**)&rta, &length);
		printf("%s\n", rta);
		free(rta);
	}
}