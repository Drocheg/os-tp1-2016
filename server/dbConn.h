#ifndef DBCONN_H
#define DBCONN_H

typedef struct shared_conn_t* SharedDBConnection;

/**
 * Creates a shared database connection.
 * 
 * @param outFD The file descriptor of a pipe for processes to write to the
 * database server.
 * @param inFD The file descriptor of a pipe for processes to read from the
 * database server.
 * @return The created database connection or NULL on error.
 */
SharedDBConnection sh_conn_create(int outFD, int inFD);

/**
 * Destroys the specified shared connection. <b>NOTE:</b> No other process will
 * be able to use this connection.
 * 
 * @return 1 on success, 0 on error.
 */
int sh_conn_destroy(SharedDBConnection c);

/**
 * Gets the fd through which to write to the database server. <b>WARNING:</b> Do
 * not call this function without having opened the connection first. Other
 * processes may be using it.
 * 
 * @param c The <b>opened</b> connection.
 * @return The out file descriptor.
 */
int sh_conn_get_out_fd(SharedDBConnection c);

/**
 * Gets the fd from which to read from the database server. <b>WARNING:</b> Do
 * not call this function without having opened the connection first. Other
 * processes may be using it.
 * 
 * @param c The <b>opened</b> connection.
 * @return The in file descriptor.
 */
int sh_conn_get_in_fd(SharedDBConnection c);

/**
 * Opens the specified shared connection. <b>WILL LOCK</b> if the connection is
 * busy until it frees up. When opened, the shared connection's file descriptors
 * can be used.
 * 
 * @param c The connection to open.
 * @return 1 on success, 0 on error.
 */
int sh_conn_open(SharedDBConnection c);

/**
 * Closes the specified shared connection, allowing other processes to use it.
 * 
 * @param c The connection to close.
 * @return 
 */
int sh_conn_close(SharedDBConnection c);

#endif /* DBCONN_H */

