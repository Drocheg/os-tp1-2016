#ifndef comm_h
#define comm_h

#include <sys/types.h>

/*
 * Structure for storing connection information. The definition of the structure
 * is dependent on the communication method used (FIFOs or Sockets), so it's
 * defined in more than one file. When compiling the system, the user decides
 * which communication method to use, so only one of the definitions gets 
 * compiled for any given system run.
 */
typedef struct connection_t* Connection;

/*
 * Standard messages to communicate different statuses or actions.
 */
#define MESSAGE_OK "KCOOL"
#define MESSAGE_ERROR "OHNOES"
#define MESSAGE_CLOSE "KTHXBAI"

/*
 * Creates a connection between the current process and a different one.
 * Depending on what the current process is acting at, the other endpoint can be
 * the main server, a forked server or a client.
 *
 * @param const char *address A formatted address of the other endpoint of the
 * connection to establish.
 * @return Connection The connection, ready to transmit and receive data, or
 * NULL on error.
 */
Connection conn_open(const char* address);

/*
 * Sends MESSAGE_CLOSE to the other endpoint of this connection and frees up the
 * resources used by this process to maintain the specified connection.
 *
 * @return int 1 on success, 0 on error.
 * @see MESSAGE_CLOSE
 */
int conn_close(Connection connection);

/*
 * Sends the specified message to the other endpoint of the specified connection.
 * To expect a response, call <i>conn_receive()</i> afterwards.
 *
 * @param const Connection c The connection through which to send data.
 * @param const void* data The data to send.
 * @param const size_t length The length (in bytes) of the data to send. This is
 * sent before sending the data, so the receiving end knows how many bytes to
 * read.
 * @return int 1 on success, 0 on error.
 */
int conn_send(const Connection connection, const void* data, const size_t length);

/*
 * Reads data from the specified connection.
 *
 * @param const Connection c The connection from which to read data.
 * @param void** data Where to store the received data.
 * @param size_t* length Where to store the length of the received data.
 * This is read first, in order to allocate just enough memory for the received
 * data.
 * @return int 1 on success, 0 on error.
 */
int conn_receive(const Connection conn, void** data, size_t* length);


#endif /* comm_h */
