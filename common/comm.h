#ifndef comm_h
#define comm_h

#include <sys/types.h>

struct connection_t {
    char* outFIFOPath;
    char* inFIFOPath;
    int outFD;
    int inFD;
};	//TODO move, shouldn't be here

typedef struct connection_t* Connection;

#define MESSAGE_CLOSE "KTHXBAI"
#define MESSAGE_OK "KCOOL"

//FIFOs
/*
 * The general idea for OPENING a connection is as follows:
 * 1) Client creates FIFO connection with the server (path defined in config file).
 * 2) Client creates 2 FIFOs and sends the two file paths to the server (one for sending
 *      and other for receiving data).
 * 3) Server opens the 2 FIFOs (one for read and other for write) and forks (listening to them).
 * 4) Client also opens the 2 FIFOs.
 * 5) Client and Server can now communicate.
 */

//SOCKETS
/*
 * The general idea for OPENING a connection is as follows:
 * 1) Client creates socket connection with the server via localhost (server port defined in config file).
 * 2) The server accepts the connection using accept and forks itself to service that one client.
 * 3) Client and server can now communicate.
 */


/*
 * Creates a connection between a client and a server.
 */
Connection conn_open(const char* address);

//int conn_accept(Connection connection);
//
//Connection conn_setup(int ip, int port);

/*
 * Closes a connection (i.e. finishes connection)
 * Returns 0 upon success. Otherwise, -1 is returned and errno
 * is set to indicate the error.
 */
int conn_close(Connection connection);

/*
 * Sends the specified message to the other endpoint of the specified connection. Asynchronous. To expect a response, call receive() afterwards.
 * 1) Sends message length (4 bytes little-endian integer)
 * 2) Sends message data (of the specified length)
 * Returns 0 on success or some number on error.
 */
int conn_send(const Connection connection, const void* data, size_t length);

/*
 * Awaits to receive a message from the other endpoint.
 */
int conn_receive(const Connection conn, void** data, size_t* length);
// int length;
// char* data;
// receive(conn, &data, &length);
// dentro de la función: *data = malloc(length);
// luego, para leer
//data[0]

#endif /* comm_h */
