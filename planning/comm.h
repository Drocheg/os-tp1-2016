#ifndef comm_h
#define comm_h

typedef struct server_t* Server
typedef struct connection_t * Connection
typedef struct request_t * Request;
typedef struct response_T * Response;
typedef struct message_t * Message /* Maybe we could use the same structre for request and response */



/*
 * Creates a connection between a client and a server
 */
Connection openConnection(Server server);

/*
 * Closes a connection (i.e. finishes connection)
 */
void closeConnection(Connection connection);

/*
 * Makes a request through connection, sending a message
 * Waits till the server responses
 */
Message request(Connection connection, Message message);

/*
 * Listens to request in a specific connection
 */
void listenToRequest(Connection connection);

#endif /* comm_h */