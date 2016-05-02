/* 
 * File:   database.h
 * Author: juan_
 *
 * Created on April 13, 2016, 10:07 AM
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "../SQLite/src/sqlite3.h"
#include <product.h>

typedef int (*databaseCallback)(void*, int, char**, char**);

/**
 * Opens a connection to the database.
 * 
 * @return sqlite3* The SQLite3 connection.
 */
sqlite3 *db_connect();

/**
 * Runs a custom query on the database.
 * 
 * @param connection A connection to the database.
 * @param query The query to run.
 * @param callback A callback function to run on every retrieved row.
 * @param firstCallbackArgument The first argument to pass to the callback
 * function.
 * @param errorMsgDest Where to store any error messages. <b>NOTE: </b> If an
 * error does occur, call <i>sqlite3_free(errorMsgDest)</i> to free resources.
 * @return int A success or error code, according to <i>sqlite3_exec</i>
 * @see sqlite3_exec
 */
int db_run_query(sqlite3* connection, const char* query, databaseCallback callback, void* firstCallbackArgument, char **errorMsgDest);

/**
 * Queries the database and returns all products, including those with no stock.
 * 
 * @param connection A database connection through which to get data.
 * @param destArray Where to store the retrieved products.
 * @return int The number of retrieved products.
 */
int db_get_products(sqlite3* connection, Product **destArray);

/**
 * Closes a connection to the database.
 * 
 * @param connection The connection to close and free resources for.
 * @return SQLITE_OK on success, or SQLITE_BUSY if the connection is handling
 * other requests and wasn't closed.
 */
int db_disconnect(sqlite3 *connection);

#endif /* DATABASE_H */

