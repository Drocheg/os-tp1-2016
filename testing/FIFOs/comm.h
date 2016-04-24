#ifndef comm_h
#define comm_h

#include <sys/types.h>

typedef struct connection_t* Connection;

#define MESSAGE_CLOSE "KTHXBAI"
#define MESSAGE_OK "KCOOL"

/*
 * Creates a connection between the current process and a server.
 *
 * @param const char *address The address of the main server.
 * @return Connection The connection, ready to receive data with
 * <i>conn_send</i>, or NULL on error.
 */
Connection conn_open(const char* address);

/*
 * Sends MESSAGE_CLOSE to the other endpoint of this connection
 * and closes this process' end of the connection.
 *
 * @return int 1 on success, 0 on error.
 */
int conn_close(Connection connection);

/*
 * Sends the specified message to the other endpoint of the specified connection.
 * To expect a response, call <i>conn_receive()</i> afterwards.
 *
 * @param const Connection c The connection through which to send data.
 * @param const void* data The data to send.
 * @param const size_t length The length (in bytes) of the data to send. This is
 * sent before sending the data, so the receiving end knows how much to read.
 * @return int 1 on success, 0 on error.
 */
int conn_send(const Connection connection, const void* data, const size_t length);

/*
 * Waits to read data from the specified connection.
 *
 * @param const Connection c The connection from which to read data.
 * @param void** data Where to store the received data.
 * @param size_t* length Where to store the length of the received data.
 * This is read first before reading the data itself, in order to allocate
 * just enough memory.
 * @return int 1 on success, 0 on error.
 */
int conn_receive(const Connection connection, void** data, size_t* length);


#endif /* comm_h */
