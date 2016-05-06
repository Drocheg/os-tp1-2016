#include "database.h"
#include <comm.h>
#include <stdio.h>
#include <stdlib.h>

typedef int (*databaseCallback)(void*, int, char**, char**);

static void attendConnection(Connection c);
static int countRows(int *result, int numCols, char **colData, char **colName);
static int loadProduct(Product *array, int numCols, char **colData, char **colName);

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
 * Closes a connection to the database.
 * 
 * @param connection The connection to close and free resources for.
 * @return SQLITE_OK on success, or SQLITE_BUSY if the connection is handling
 * other requests and wasn't closed.
 */
int db_disconnect(sqlite3 *connection);

int main(int argc, char **argv) {
    char *listeningAddress = "/tmp/dbFIFO";
    ConnectionParams connParams = conn_listen(listeningAddress);
    if (connParams == NULL) {
        fprintf(stderr, "Couldn't set up requests FIFO in database server. Aborting.\n");
        remove(listeningAddress);
        return -1;
    }

    printf("This is database server #%i, listening to connections at %s\n", getpid(), listeningAddress);
    //Main loop
    while (1) {
        printf("Waiting for clients...");
        fflush(stdout);
        Connection c = conn_accept(connParams);
        if(c == NULL) { //TODO
            printf("Error accepting connection. Ignoring?\n");
            continue;
        }
        printf("client connected.\n");
        //No forkin' forks! One connection at a time, one request per connection
        attendConnection(c);
        conn_close(c);      //TODO will send MESSAGE_CLOSE but it's unnecessary? If the client sends MESSAGE_CLOSE too it will block
    }
    return 0;
}

int db_run_query(sqlite3* connection, const char* query, databaseCallback callback, void* firstCallbackArgument, char **errorMsgDest) {
    return sqlite3_exec(connection, query, callback, firstCallbackArgument, errorMsgDest);
}

int db_get_products(sqlite3* connection, Product **destArray) {
    char *errormsg;
    int numProducts;
    db_run_query(connection, "SELECT count(id) FROM products", (databaseCallback) countRows, &numProducts, &errormsg);
    if (numProducts <= 0) {
        return -1;
    }
    *destArray = calloc(numProducts, sizeof (*destArray));
    db_run_query(connection, "SELECT name, description, price, quantity FROM products", (databaseCallback) loadProduct, *destArray, &errormsg);
    return numProducts;
}

int db_place_order(sqlite3* connection, Order order) {
    //TODO
    return 0;
}

sqlite3 *db_connect() {
    sqlite3 *result;
    int number = sqlite3_open("db", &result); //Create temporary database
    if (number) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(result));
        return NULL;
    }
    return result;
}

int db_disconnect(sqlite3 *connection) {
    return sqlite3_close(connection);
}

static void attendConnection(Connection c) {
    //TODO read request from connection
    //TODO carry out request
}

static int countRows(int *result, int numCols, char **colData, char **colName) {
    *result = strtol(colData[0], NULL, 10);
    return 0;
}

static int loadProduct(Product *array, int numCols, char **colData, char **colName) {
    int i = 0;
    //TODO don't go to end of array on every item
    while (array[i] != NULL) //Append to end of array
        i++;
    array[i] = newProduct(colData[0], colData[1], atof(colData[2]), atoi(colData[3]));
    return 0;
}