#include "forkedServer.h"
#include <sys/types.h>
#include "aux.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int forkedServer(Connection connection) {


	char * rta;
	size_t length;
	char * started = "Connection started";
	pid_t pid = getpid();
	printf("%s\n", started);
	printf("PID: %d\n", pid);
	
	conn_send(connection, started, strlen(started) + 1);
	conn_send(connection, (void*)&pid, sizeof(pid_t));
	while(1) {

		conn_receive(connection, (void**)&rta, &length);
		char * aux = "OK!";
		conn_send(connection, aux, strlen(aux) + 1);
		printf("%s\n", rta);
	}
}