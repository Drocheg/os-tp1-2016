#include "comm.h"
#include "lib.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <regex.h> 
#include <stdio.h>

struct connection_t {
    char *ip;
    in_port_t port;
    int socketfd;

};

struct conn_params_t {
    in_port_t port;
    int socketfd;

};


/* Connecting functions wrappers */
static int inet_pton_wrp(int af, const char* src, void* dst);
static int socket_wrp(int domain, int type, int protocol);
static int connect_wrp(int socket, const struct sockaddr* address, socklen_t address_len);
static int bind_wrp(int socket, const struct sockaddr* address, socklen_t address_len);
static int listen_wrp(int socket, int backlog);
static int shutdown_wrp(int socket, int how);

/* Auxiliar functions */
static int searchForColon(const char *address);
static char* getIPRegEx();
static char* getPortRegEx();
static int checkIP(const char *ip);
static int checkPort(const char *portStr);
static int isValidAddress(const char *address);
static int initializeClient(Connection connection, const char *address);
static int initializeServer(ConnectionParams connectionParams, char *listeningAddress);
static int createTCPSocketForClient(Connection connection);
static int createTCPSocketForServer(ConnectionParams connection);




/*****************/
/* API Functions */
/*****************/

/*
 * Opens a connection with the specified address. Another process should have
 * called <i>conn_open</i> with the same address for the connection to establish. 
 *
 * @param const char *address A formatted address where to listen to requests.
 * @return Connection The connection on success, or NULL on error.
 */
Connection conn_open(const char* address) {

    Connection connection;
    int result;

    if (!isValidAddress(address)) {
        return NULL;
    }

    connection = malloc(sizeof (*connection));
    if (connection == NULL) {
        return NULL;
    }

    return (initializeClient(connection, address) == -1) ? NULL : connection;

}

/*
 * Sends MESSAGE_CLOSE to the other endpoint of this connection and frees up the
 * resources used by this process to maintain the specified connection.
 *
 * @return int 1 on success, 0 on error.
 * @see MESSAGE_CLOSE
 */
int conn_close(Connection connection) {
    
    int message = MESSAGE_CLOSE;

    if (!conn_send(connection, &message, sizeof(message))) {
        return 0; /* Couldn't communicate with server to close */
    }
    if (shutdown_wrp(connection->socketfd, SHUT_RDWR)) {
        return 0; /* Couldn't close connection */
    }
    free(connection->ip);
    free(connection);
    return 1;

}

/*
 * Sends the specified message to the other endpoint of the specified connection.
 * To expect a response, call <i>conn_receive</i> afterwards.
 *
 * @param const Connection c The connection through which to send data.
 * @param const void* data The data to send.
 * @param const size_t length The length (in bytes) of the data to send. This is
 * sent before sending the data, so the receiving end knows how many bytes to
 * read.
 * @return int 1 on success, 0 on error.
 */
int conn_send(const Connection connection, const void* data, const size_t length) {

    if (length == 0) {
        return 1;
    }
    return ensureWrite(&length, sizeof (length), connection->socketfd) && ensureWrite(data, length, connection->socketfd);
}

/*
 * Reads data from the specified connection.
 *
 * @param const Connection c The connection from which to read data.
 * @param void** data Where to store the received data.
 * @param size_t* length Where to store the length of the received data. Can be
 * NULL if this is not needed (e.g. if the length of the data to receive is
 * known)
 * This is read first, in order to allocate just enough memory for the received
 * data.
 * @return int 1 on success, 0 on error.
 */
int conn_receive(const Connection conn, void** data, size_t* length) {

    size_t lenAux;
    if (length == NULL) { //If there's nowhere to store the read length, store it within the function, then discard
        length = &lenAux;
    }
    int result1, result2;
    if (!ensureRead(length, sizeof (*length), conn->socketfd)) {
        return 0;
    }
    *data = malloc(*length);
    return ensureRead(*data, *length, conn->socketfd);
}

/**
 * Sets up the current process to listen for connections at the specified
 * address.
 *
 * @param char* listeningAddress Where the process should listen for connections.
 * For connections to go through, a different process should call <i>conn_open()</i>
 * specifying the same address.
 * @return ConnectionParams A structure of connection parameters needed for the
 * process to actually start accepting connections.
 */
ConnectionParams conn_listen(char *listeningAddress) {

    ConnectionParams connectionParams;

    if (!checkPort(listeningAddress)) {
        return NULL;
    }

    connectionParams = malloc(sizeof (*connectionParams));
    if (connectionParams == NULL) {
        return NULL;
    }
    return (initializeServer(connectionParams, listeningAddress) == -1) ? NULL : connectionParams;
}

/**
 * Waits until there's an available connection, and returns it when it's ready.
 * 
 * @param ConnectionParams params Parameters for listening to connection requests.
 * Dependent on the communication method used.
 * @return Connection An established connection, ready to receive and send data,
 * or NULL on error.
 */
Connection conn_accept(ConnectionParams params) {

    int newFD = accept(params->socketfd, NULL, NULL);
    Connection connection;
    if (newFD < 0) {
        return NULL; /* Couldn't create a new FD for the connection */
    }
    connection = malloc(sizeof (*connection));
    if (connection == NULL) {
        return NULL; /* Couldn't allocate memory for the connection struct */
    }

    connection->ip = NULL;
    connection->port = params->port;
    connection->socketfd = newFD;

    return connection;
}


/**********************/
/* Auxiliar functions */
/**********************/

/*
 * Searches for a colon (':') in the address
 * Returns the position in which the colon was found, or -1 if not found
 */
static int searchForColon(const char *address) {

    int i = 0;
    while (address[i] != 0 && address[i] != ':') {
        i++;
    }
    return (address[i] == 0) ? -1 : i;
}

/*
 * Returns a string with the ip regex
 * Allocates memory
 */
static char* getIPRegEx() {

    char *expr1 = "^((0|[3-9][0-9]?|1[0-9]?[0-9]?|25[0-5]|2[0-4]?[0-9]?|2[0-9])\\.)";
    char *expr2 = "{1,3}(0|[3-9][0-9]?|1[0-9]?[0-9]?|25[0-5]|2[0-4]?[0-9]?|2[0-9])$";
    size_t len1 = strlen(expr1);
    size_t len2 = strlen(expr2);
    size_t totalLength = len1 + len2;

    char *expr = calloc(1, (totalLength + 1) * sizeof (char));
    if (expr == NULL) {
        return NULL;
    }
    memcpy(expr, expr1, len1 * sizeof (char));
    memcpy(expr + len1, expr2, len2 * sizeof (char));

    return expr;
}

/*
 * Returns a string with the port regex
 */
static char* getPortRegEx() {

    return "^(0|[1-9][0-9]{1,4})$";
}

/*
 * Checks if an ip address is valid (just the address part, not the port)
 * Returns 1 if it was a valid ip, or 0 otherwise
 * Returns -1 if an error ocurred (i.e. memory couldn't be allocated for the regex)
 */
static int checkIP(const char *ip) {

    regex_t regex;
    char *expr = getIPRegEx();
    int result = 0;

    if (expr == NULL) {
        return -1;
    }

    regcomp(&regex, expr, REG_EXTENDED);
    result = !regexec(&regex, ip, 0, NULL, 0); /*Returns 0 if success, or a non-zero code if not match */
    free(expr); /* Must free because getIpRegex function allocates memory */
    return result;

}

/*
 * Checks if a port is valid
 * Returns 1 if it was a valid port, or 0 otherwise
 */
static int checkPort(const char *portStr) {

    regex_t regex;
    char *expr = getPortRegEx();
    int result = 0;
    unsigned short port;
    regcomp(&regex, expr, REG_EXTENDED);
    result = regexec(&regex, portStr, 0, NULL, 0); /* Returns 0 if success, or a non-zero code if not match */

    if (result) {
        return 0; /* Port doesn't match with specified format */
    }
    port = atoi(portStr);
    if (port > 65535) { /* Just check upper limit because a negative number wouldn't have passsed the regex test */
        return 0; /* Port out of range*/
    }
    return 1;
}

/*
 * Checks that the address is valid
 * An address is valid if it has the following format: ddd.ddd.ddd.ddd:ppppp
 * Returns 1 if it was a valid address, or 0 otherwise
 * Returns -1 in case of error (i.e. memory couldn't be allocated for a regex)
 */
static int isValidAddress(const char *address) {

    int colonPosition = searchForColon(address);
    char *ipPart;
    char *portPart;
    int portPartLength = strlen(address) - colonPosition - 1;
    int flag1 = 0, flag2 = 0;

    if (colonPosition < 0) {
        return 0; /* Invalid IP Address. Colon is missing */
    }

    ipPart = calloc(1, (colonPosition + 1) * sizeof (char));
    portPart = calloc(1, (portPartLength + 1) * sizeof (char));
    memcpy(ipPart, address, colonPosition * sizeof (char));
    memcpy(portPart, address + colonPosition + 1, portPartLength * sizeof (char));

    flag1 = checkIP(ipPart);
    flag2 = checkPort(portPart);

    if (flag1 == -1) {
        return -1;
    }

    return flag1 && flag2;
}

/*
 * Creates the TCP Socket for a client connection
 * Returns 0 on success, -1 otherwise
 */
static int createTCPSocketForClient(Connection connection) {

    struct sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(connection->port);

    if (inet_pton_wrp(AF_INET, connection->ip, &serverAddress.sin_addr)) {
        return -1;
    }
    if ((connection->socketfd = socket_wrp(PF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    if (connect_wrp(connection->socketfd, (struct sockaddr*) &serverAddress, sizeof (serverAddress))) {
        return -1;
    }
    return 0;
}

/*
 * Initializes a client connection
 * Returns 0 on sucess, or -1 otherwise
 */
static int initializeClient(Connection connection, const char *address) {

    int colonPosition = searchForColon(address);
    connection->ip = calloc(1, (colonPosition + 1) * sizeof (char));

    if (connection->ip == NULL) {
        return -1;
    }
    memcpy(connection->ip, address, colonPosition * sizeof (char));
    connection->port = (in_port_t) atoi((char *) (((void *) address) + colonPosition + 1));

    return createTCPSocketForClient(connection);

}

/*
 * Creates the TCP Socket for a server connection
 * Returns 0 on success, -1 otherwise
 */
static int createTCPSocketForServer(ConnectionParams connectionParams) {

    struct sockaddr_in mainServer;

    mainServer.sin_family = AF_INET;
    mainServer.sin_addr.s_addr = htonl(INADDR_ANY); /* Enables connection through all present interfaces */
    mainServer.sin_port = htons(connectionParams->port);

    if ((connectionParams->socketfd = socket_wrp(PF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    if (bind_wrp(connectionParams->socketfd, (struct sockaddr*) &mainServer, sizeof (mainServer))) {
        return -1;
    }
    if (listen_wrp(connectionParams->socketfd, 10)) {
        return -1;
    }

    return 0;
}

/*
 * Initializes a server connection
 * Returns 0 on sucess, or -1 otherwise
 */
static int initializeServer(ConnectionParams connectionParams, char *listeningAddress) {

    connectionParams->port = atoi(listeningAddress);
    return createTCPSocketForServer(connectionParams);
}



/*********************************/
/* Connecting functions wrappers */

/*********************************/
static int inet_pton_wrp(int af, const char* src, void* dst) {

    printf("%s\n", src);
    int result = inet_pton(af, src, dst);
    if (result == 0) {
        fprintf(stderr, "Invalid IP address\n");
        return -1;
    }
    if (result == -1) {
        fprintf(stderr, "Couldn't parse IP. System error\n");
        return -1;
    }
    return 0;
}

static int socket_wrp(int domain, int type, int protocol) {

    int result = socket(domain, type, protocol);
    if (result < 0) {
        fprintf(stderr, "Couldn't create socket\n");
    }
    return result;
}

static int connect_wrp(int socket, const struct sockaddr* address, socklen_t address_len) {

    if (connect(socket, address, address_len)) {
        fprintf(stderr, "Couldn't create connection\n");
        return -1;
    }
    return 0;
}

static int bind_wrp(int socket, const struct sockaddr* address, socklen_t address_len) {

    if (bind(socket, address, address_len)) {
        fprintf(stderr, "Couldn't bind socket\n");
        return -1;
    }
    return 0;
}

static int listen_wrp(int socket, int backlog) {

    if (listen(socket, backlog)) {
        fprintf(stderr, "Can't listen through socket\n");
        return -1;
    }
    return 0;
}

static int shutdown_wrp(int socket, int how) {

    if (shutdown(socket, how)) {
        fprintf(stderr, "Couldn't close connection\n");
        return -1;
    }
    return 0;
}