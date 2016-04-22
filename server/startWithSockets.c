#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include "../common/data.h"

typedef struct srv_connection_t * ServerConnection;


struct srv_connection_t {	
	uint16_t port;
	int listenfd;
	struct sockaddr_in mainServer;
	socklen_t socklen;

};



int readFromConnection(int fd, void** data, size_t* length) {
    
	read(fd, length, sizeof(size_t));
	return return !(*length - read(fd, *data, *length));
}

static void writeToConnection(int fd, Data data) {
    
    size_t size = getSize(data);

	write(fd, &size, sizeof(size_t));
	write(fd, getData(data), getSize(data));
}

static void forkedServer(int socket_fd) {

	while(1) {
		Data request = readFromConnection(socket_fd);
        Data response = newData((void *) "[OK]", sizeof("[OK]"), STRING);
		writeToConnection(socket_fd, response);
	}
}





// static ServerConnection new_connection(char * port) {

// 	ServerConnection serverConnection = malloc(sizeof(*serverConnection));
// 	serverConnection->port = (uint16_t) atoi(port);
// 	serverConnection->listenfd = socket(PF_INET, SOCK_STREAM, 0);
// 	serverConnection->socklen = sizeof(serverConnection->mainServer);

// 	serverConnection->mainServer.sin_family = AF_INET;
//     serverConnection->mainServer.sin_addr.s_addr = htonl(INADDR_ANY); /* Enables connection through all present interfaces */
//     serverConnection->mainServer.sin_port = htons(serverConnection->port);

// 	return serverConnection;
// }

// /*
//  * Sets up connection
//  * Return 0 upon success, -1 if bind couldn't be done, or -2 if couldn't start listening
//  */
// static int setup(char * port, ServerConnection * serverConnection) {

// 	*serverConnection = new_connection(port);

// 	if (bind((*serverConnection)->listenfd,
// 			(struct sockaddr*)&(*serverConnection)->mainServer,
// 			(*serverConnection)->socklen)) {
//     	fprintf(stderr, "Couldn't bind socket\n");
//     	return -1;
//     }

//     if (listen((*serverConnection)->listenfd, 10)) {
//    		fprintf(stderr, "Can't listen through socket\n");
//    		return -2;
//    	}
//    	return 0;


// }


int start(char * address) {

	conn_open(address);
}