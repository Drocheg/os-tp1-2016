#include "comm.h"
#include "config.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#ifndef ATTEMPTS
#define ATTEMPTS 10
#endif

#ifndef BLOCK
#define BLOCK 1024
#endif


struct connection_t {
	char * ip;
	unsigned short port;
	int socketfd;

};

/*
 * Gets the IP part from the address string, assuming the ddd.ddd.ddd.ddd:ppppp 
 * format, where ddd is a decimal number between 0 and 255, and ppppp is a decimal
 * number from 0 to 65535
 */
static char * getIP(const char * address) {

	char * aux;
	int i = 0;
	
	while (address[i++] != ':')
		;
	aux = malloc(i);
	memcpy(aux, (void*)address, i - 1);
	aux[i] = 0;
	
	return aux;
}

/*
 * Gets the port number from the address string
 * Assumes address is NULL-terminated
 */
static unsigned short getPort(const char * address) {

	int i = 0;
	while (address[i++] != ':')
		;
    address += i;
    return (unsigned short)atoi((address));

}



Connection conn_open(const char* address) {

	Connection connection = malloc(sizeof(*connection));
	struct sockaddr_in server_ip;

	connection->ip = getIP(address);
	connection->port = getPort(address);
	connection->socketfd = socket(PF_INET, SOCK_STREAM, 0);

	server_ip.sin_family = AF_INET;
	server_ip.sin_port = htons((in_port_t)connection->port);
	inet_pton(AF_INET, connection->ip, &server_ip.sin_addr);
	connect(connection->socketfd, (struct sockaddr*)&server_ip, sizeof(server_ip));


	return connection;

}

int conn_close(Connection connection) {
	conn_send(connection, CLOSE_MESSAGE, sizeof(CLOSE_MESSAGE));
	free(connection);
	return 1;
}


int conn_send(const Connection connection, const void* data, const size_t length) {

	int i = 0;
	write(connection->socketfd, &length, sizeof(size_t));
	while (i++ < ATTEMPTS) {
		if (!(length - write(connection->socketfd, data, length))) {
			return 1;
		}
	}
	return 0;
}


int conn_receive(const Connection connection, char** data, size_t* length) {

	read(connection->socketfd, length, sizeof(size_t));
	*data = malloc(*length);
	return !(*length - read(connection->socketfd, *data, *length));
}
