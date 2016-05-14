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

typedef struct conn_params_t* ConnectionParams;

/*
 * Standard messages to communicate different statuses or actions.
 */
#define MESSAGE_OK "KCOOL"
#define MESSAGE_ERROR "OHNOES"
#define MESSAGE_CLOSE "KTHXBAI"
#define MESSAGE_BUY "GIMMEDIS"
#define MESSAGE_LIST_START "WHATCHUGOT"
#define CMD_GET_PRODUCTS 42
#define CMD_PLACE_ORDER 43
#define CMD_NO_OP 12345


/*
 * Opens a connection with the specified address. Another process should have
 * called <i>conn_open</i> with the same address for the connection to establish. 
 *
 * @param const char *address A formatted address where to listen to requests.
 * @return Connection The connection on success, or NULL on error.
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
 * To expect a response, call <i>conn_receive</i> afterwards.
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
 * @param size_t* length Where to store the length of the received data. Can be
 * NULL if this is not needed (e.g. if the length of the data to receive is
 * known)
 * This is read first, in order to allocate just enough memory for the received
 * data.
 * @return int 1 on success, 0 on error.
 */
int conn_receive(const Connection conn, void** data, size_t* length);

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
ConnectionParams conn_listen(char *listeningAddress);

/**
 * Waits until there's an available connection, and returns it when it's ready.
 * 
 * @param ConnectionParams params Parameters for listening to connection requests.
 * Dependent on the communication method used.
 * @return Connection An established connection, ready to receive and send data,
 * or NULL on error.
*/
Connection conn_accept(ConnectionParams params);


#endif /* comm_h */
