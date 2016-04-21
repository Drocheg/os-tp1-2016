#include "comm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char * getLine() {

	char * aux = NULL;
	char * result = NULL;
	int i = 0;
	do {
		if (i % 10 == 0) {
			aux = realloc(result, 10);
			result = aux;
		}
		result[i] = getchar();

	} while (aux[i++] != '\n')
		;
	result[i - 1] = 0;
	return result;
}


int main(int argc, char *argv[]) {

	Connection connection = conn_open(argv[1]);
	char * rta;
	size_t length;
	while(1) {

		char * aux = getLine();
		conn_send(connection, aux, strlen(aux) + 1);
		conn_receive(connection, &rta, &length);
		printf("%s\n", rta);
	}
}