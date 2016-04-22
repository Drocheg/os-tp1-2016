#include "comm.h"
#include "config.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <regex.h> 
#include <stdio.h>
#include "../server/forkedServer.h"

#ifndef ATTEMPTS
#define ATTEMPTS 10
#endif

#ifndef BLOCK
#define BLOCK 1024
#endif


#ifndef MAIN_SERVER_CONNECTION
#define MAIN_SERVER_CONNECTION 1
#endif

#ifndef FORKED_SERVER_CONNECTION
#define FORKED_SERVER_CONNECTION 2
#endif

#ifndef CLIENT_CONNECTION
#define CLIENT_CONNECTION 3
#endif



struct connection_t {
	char* ip;
	in_port_t port;
	int socketfd;

};

/*
 * Gets the IP part from the address string, assuming the ddd.ddd.ddd.ddd:ppppp 
 * format, where ddd is a decimal number between 0 and 255, and ppppp is a decimal
 * number from 0 to 65535
 * Returns the IP address or NULL if there isn't a match with specific format
 */
static char * getIP(const char * address) {

	char * aux = NULL;
	int i = 0, flag = 0;
	
	while (address[i] != ':' && i < 16 && (flag = !(address[i] != 0))) {
		i++;
	} 
	if (i < 16 && !flag) {
		aux = malloc(i + 1);
		memcpy(aux, (void*)address, i);
		aux[i + 1] = 0;
	}
	
	return aux;
}

/*
 * Gets the port number from the address string (format: ddd.ddd.ddd.ddd:ppppp)
 * Assumes address is NULL-terminated
 */


static in_port_t getPortFromAddress(const char* address) {

	int i = 0;
	while (address[i++] != ':')
		;
    address += i;
    return (in_port_t)atoi((address));
}

static in_port_t getPortFromCSV(const char* address) {

	char * port;
	int i = 0;
	while(address[i] != ',') {
		i++;
	} 
	port = malloc(i - 1);
	memcpy(port, address + 2, i - 1);
	port[i] = 0;
	return (in_port_t)atoi(port);
}

static in_port_t getPort(const char* address) {

	if(address[0] == 'p') {
		return getPortFromCSV(address); /* This means that the address string is a csv (forked servers) */
	}

	if (strlen(address) < 6) {
		return atoi(address); /* This means that the address string has only the port number */
	}
	return getPortFromAddress(address);

}


static int getFDFromCSV(const char* address) {
	
	int i = 0;
	while(address[i] != ',' && address[i] != 0) {
		i++;
	}
	if (address[i] == 0) {
		return -1;
	}
	return atoi(address + i + 3);
}



static int isPort(const char* address) {

	int port;
    regex_t regex;
    regcomp(&regex, "^(0|[1-9][0-9]{1,4})$", REG_EXTENDED);
    if (regexec(&regex, address, 0, NULL, 0)) {
        return 0;
    }

	port = atoi(address);
	if (port < 0 || port > 65535) {
		return 0;
	}
	return 1;
}


static int isIP(const char* address) {

	regex_t regex;
	char * expr = "^((0|[3-9][0-9]?|1[0-9]?[0-9]?|25[0-5]|2[0-4]?[0-9]?|2[0-9])\\.){1,3}(0|[3-9][0-9]?|1[0-9]?[0-9]?|25[0-5]|2[0-4]?[0-9]?|2[0-9])$";
    regcomp(&regex, expr, REG_EXTENDED);
    
    return regexec(&regex, address, 0, NULL, 0);
}


static int isIPAndPort(const char* address) {

	int i = 0;
	char* ip;
	char* port;
	while(address[i] != ':' && i < 15) {
		i++;
	}
	if (address[i] != ':') {
		return 0;
	}
	ip = malloc(i);
	memcpy(ip, address, i);
	ip[i] = 0;
	if (!isIP(ip)) {
		return 0;
	}

	port = (char*)((void*)address + i + 1);
	if (!isPort(port)) {
		return 0;
	}
	return 1;

}





static int isForkedServerAddressType(const char* address) {

	int i = 0, port;
	char* aux;

	while (address[i] != ',' && address[i] != 0) {
		i++;
	}
	if (address[i] == 0) {
		return 0;
	}

	aux = malloc(i);
	aux = strncpy(aux, address, i);
    regex_t regex;
    char* expr = "^p=(0|[1-9][0-9]{1,4})$";
    regcomp(&regex, expr, REG_EXTENDED);
    if (regexec(&regex, aux, 0, NULL, 0)) {
        return 0;
    }
	port = atoi( (char*)(void*)address + 2);
	if (port < 0 || port > 65535) {
		return 0;
	}

	expr = "^f=(0|[1-9][0-9]*)$";
	regcomp(&regex, expr, REG_EXTENDED);
	return !regexec(&regex, (char*)((void*)address + i + 1) , 0, NULL, 0);

}


/*
 * Address type can be one of the following formats:
 * 		ddd.ddd.ddd.ddd:ppppp	This is a normal IP Address and the port number (clients start connections with them)
 *		ppppp					Only port number (for main server connection)
 * 		p=ppppp, f=fffff		Port and file descriptor through which a forked server can communicate (for forked servers)
 * 
 * Main servers create a connection within themselves
 */
static int checkAddressType(const char* address) {

	if (isPort(address)) {
		return MAIN_SERVER_CONNECTION;
	}

	if(isForkedServerAddressType(address)) {
		return FORKED_SERVER_CONNECTION;
	}
	if(isIPAndPort(address)) {
		return CLIENT_CONNECTION;
	}
	return 0;
}







// Connection client_conn_open(const char* address) {

// 	Connection connection = malloc(sizeof(*connection));
// 	struct sockaddr_in serverAddress;

// 	connection->ip = getIP(address);
// 	connection->port = getPort(address);
// 	connection->socketfd = socket(PF_INET, SOCK_STREAM, 0);

// 	serverAddress.sin_family = AF_INET;
// 	serverAddress.sin_port = htons((in_port_t)connection->port);
// 	inet_pton(AF_INET, connection->ip, &serverAddress.sin_addr);
// 	connect(connection->socketfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));


// 	return connection;

// }


static Connection client_conn_open(Connection connection) {

	struct sockaddr_in serverAddress;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(connection->port);
	inet_pton(AF_INET, connection->ip, &serverAddress.sin_addr);

	if ((connection->socketfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Couldn't create socket\n");
    	return NULL;
	}
	
	if (connect(connection->socketfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress))) {
		fprintf(stderr, "Couldn't create connection\n");
    	return NULL;
	}

	return connection;

}

static Connection forked_server_conn_open(in_port_t port, int fd) {

	Connection connection = malloc(sizeof(*connection));
	
	connection->ip = NULL;
	connection->port = port;
	connection->socketfd = fd;

	return connection;

}


static Connection main_server_conn(Connection connection) {

	struct sockaddr_in mainServer;

	mainServer.sin_family = AF_INET;
    mainServer.sin_addr.s_addr = htonl(INADDR_ANY); /* Enables connection through all present interfaces */
    mainServer.sin_port = htons(connection->port);


	if ((connection->socketfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Couldn't create socket\n");
    	return NULL;
	}

	if (bind(connection->socketfd,
			(struct sockaddr*)&mainServer,
			sizeof(mainServer))) {
    	fprintf(stderr, "Couldn't bind socket\n");
    	return NULL;
    }

    if (listen(connection->socketfd, 10)) {
   		fprintf(stderr, "Can't listen through socket\n");
   		return NULL;
   	}
   	printf("Now listening on port %hu", connection->port);
   	while(1) {
    	int new_fd = accept(connection->socketfd, NULL, NULL);
    	if (fork() > 0) {
    		/* New process created, and forkedServer function called */
			exit(forkedServer(forked_server_conn_open(connection->port, new_fd)));
		}
 
    }

	return connection;
}






Connection conn_open(const char* address) {

	int connectionType = checkAddressType(address);
	Connection connection;
	
	if (!connectionType) {
		return NULL; /* No matching pattern for address */
	}

	connection = malloc(sizeof(*connection));
	connection->ip = getIP(address);
	connection->port = getPort(address);

	switch(connectionType) {
		case MAIN_SERVER_CONNECTION: { 
			return main_server_conn(connection);
		}
		case FORKED_SERVER_CONNECTION: { 
			return forked_server_conn_open(connection->port, getFDFromCSV(address));
		}	
		case CLIENT_CONNECTION: {
			return client_conn_open(connection);
		} default: {
			return NULL;
		}

	}


}


int conn_close(Connection connection) {
	conn_send(connection, MESSAGE_CLOSE, sizeof(MESSAGE_CLOSE));
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
