#include "comm.h"
#include "config.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>




/*
 * Gets the port number from the address string
 * Assumes address is NULL-terminated
 */
static unsigned short getPort(const char * address) {

	int i = 0;
	while (address[i++] != ':');
	return (unsigned short)atoi(&(address[i + 1]));

}


static void readFromConnection(int fd, void** data) {

	int length = 0;
	read(fd, &length, sizeof(size_t));
	*data = malloc(length);
	read(fd, data, length);

}

static void writeToConnection(int fd, char * message) {

	size_t aux = strlen(message) + 1;
	write(fd, &aux, sizeof(size_t));
	write(fd, message, aux);
}

static void forkedServer(int socket_fd) {

	printf("Hi, I'm a forked server\n");

	while(1) {

		char * message;
		readFromConnection(socket_fd, (void**)&message);
		printf("%s\n", message);
		writeToConnection(socket_fd, "Copiado");

	}
}

static void start(unsigned short port) {

	struct sockaddr_in main_server_ip, forked_server_ip;
	int listenfd = socket(PF_INET, SOCK_STREAM, 0);
	socklen_t aux = sizeof(main_server_ip);
	

	main_server_ip.sin_family = AF_INET;
    main_server_ip.sin_addr.s_addr = htonl(INADDR_ANY); /* Enables connection through all present interfaces */
    main_server_ip.sin_port = htons((uint16_t)(port));


    bind(listenfd, (struct sockaddr*)&main_server_ip, sizeof(main_server_ip));
    listen(listenfd, 32);

    while(1) {
    	int new_fd = accept(listenfd, (struct sockaddr*)&forked_server_ip, &aux);
    	if (fork() > 0) {
    		printf("I'm here\n");
    		forkedServer(new_fd);
    	}
    }


}

int main(int argc, char *argv[]) {


	

	start((unsigned short)getPort(argv[1]));

	return 0;

}


