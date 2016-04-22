#include "comm.h"
#include "config.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>




static void readFromConnection(int fd, void** data) {

	size_t length = 0;
	read(fd, &length, sizeof(size_t));
	*data = malloc(length);
	read(fd, *data, length);
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
    listen(listenfd, 10);

    while(1) {
    	int new_fd = accept(listenfd, (struct sockaddr*)&forked_server_ip, &aux);
    	if (fork() > 0) {
    		forkedServer(new_fd);
    	}
    }


}

int main(int argc, char *argv[]) {

	unsigned short port = atoi(argv[1]);

	printf("Now listening on port %d\n", port);


	

	start(port);

	return 0;

}


