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
	
	while (address[i++] != ':');
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

	data = malloc(*length);
	return !(*length - read(connection->socketfd, *data, *length));

}


/*
 * The general idea for opening a connection with SOCKETS is as follows:
 * 1) Client creates socket connection with the server via localhost (server port defined in config file).
 * 2) The server accepts the connection using accept and forks itself to service that one client.
 * 3) Client and server can now communicate.
 */


//#include "comm.h"
//#include <stdlib.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <stdio.h>
//#include <unistd.h>
//#include <string.h>
//
//struct connection_t {
//    char *path;
//    int clientPID;
//    int serverPID;
//};
//
//
//Connection openConnection(Server server) {
//    
//    Connection connection = malloc(sizeof(*connection));
//    char path[16];
//    int pid = getpid();
//    sprintf(path, "/tmp/%i", pid);
//    //0666 == anybody can read and write
//    if (mkfifo(path, 0666)) {    //Error
//        fprintf(stderr, "Error creating client FIFO.");
//    }
//    printf("Successfully created FIFO at %s", path);
//    connection->path = malloc(strlen(path)+1);
//    strcpy(connection->path, path);
//    connection->clientPID = pid;
//    return connection;
//}
//
///*
// * The general idea for CLOSING a connection is as follows:
// * 1) Client/Server writes "CLOSE" on shared FIFO. This means Client/Server will
// * no longer write to/read from said FIFO. - done
// * 2) Client/Server calls remove(fifopath) - done
// * 3) When done, Server/Client and any other processes using the FIFO should
// * also call remove(fifopath) so as to actually remove the "file."
// */
//int closeConnection(Connection connection) {
//    
//    FILE *f = fopen(connection->path, "w");
//    char *msg = "CLOSE";
//    /*
//     * BLOCKING: This won't run unless there's another proces reading from the FIFO
//     * If testing this, run cat < /path/to/fifo and you'll see the data and the program will close the connection
//     */
//    fwrite(&msg, strlen(msg)+1, 1, f);
//    fclose(f);
//    remove(connection->path);
//    printf("Successfully closed FIFO %s", connection->path);
//}