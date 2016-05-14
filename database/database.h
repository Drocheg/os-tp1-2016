#ifndef DATABASE_H
#define DATABASE_H

#include "dbConn.h"
#include <sqlite3.h>
#include <product.h>
#include <order.h>

/**
 * Starts the database server which will communicate through the specified
 * shared connection.
 * 
 * @param c The shared connection through which to listen for requests.
 * @return 0 on error, otherwise returns 1 when the server shuts down normally.
 */
int db_server_start(SharedDBConnection c);


#endif /* DATABASE_H */

