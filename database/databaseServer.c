#include "database.h"
#include "dbConn.h"
#include <comm.h>
#include <logging.h>
#include <lib.h>

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef int (*databaseCallback)(void*, int, char**, char**);

const char* dbFileName = "db";
sqlite3 *databaseHandle;

/**
 * Runs a SQL query on the database.
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
int run_query(sqlite3* connection, const char* query, databaseCallback callback, void* firstCallbackArgument, char **errorMsgDest) {
    log_info(query);
    int result = sqlite3_exec(connection, query, callback, firstCallbackArgument, errorMsgDest);
    if (result != SQLITE_OK) {
        log_warn("Last query was not completed successfully.");
    }
    return result;
}

/**
 * Stores the result of a query that returns a single number at the specified
 * int pointer.
 * 
 * @param result Where to store the result.
 * @return 0
 */
int store_single_number(int *result, int numCols, char **colData, char **colName) {
    *result = strtol(colData[0], NULL, 10);
    return 0;
}

/**
 * Loads a product to the end of the specified array with the information
 * returned by a SELECT query from the products table.
 * 
 * @param array Where to store the product. <b>WARNING:</b> Array should be
 * calloc()ed, that's how the function knows when it's reached the end of the
 * array. Otherwise segmentation faults will occur.
 * 
 * @return 0
 */
int append_products(Product *array, int numCols, char **colData, char **colName) {
    int i = 0;
    while (array[i] != NULL) //Append to end of array
        i++;
    array[i] = newProduct(atoi(colData[0]), colData[1], colData[2], atof(colData[3]), atoi(colData[4]));
    return 0;
}

/**
 * Queries the database and returns all products, including those with no stock.
 * 
 * @param c The connection through which to communicate with the database server
 * @param destArray Where to store the retrieved products.
 * @return The number of retrieved products.
 */
int get_products(Product **destArray) {
    char *errormsg;
    int numProducts;
    if (run_query(databaseHandle, "SELECT count(id) FROM products", (databaseCallback) store_single_number, &numProducts, &errormsg) != SQLITE_OK) {
        log_warn(errormsg);
        sqlite3_free(errormsg);
        return -1;
    }
    if (numProducts <= 0) {
        return -1;
    }
    *destArray = calloc(numProducts, sizeof (**destArray));
    if (run_query(databaseHandle, "SELECT id, name, description, price, quantity FROM products", (databaseCallback) append_products, *destArray, &errormsg) != SQLITE_OK) {
        log_warn(errormsg);
        sqlite3_free(errormsg);
        return -1;
    }
    return numProducts;
}

/**
 * Sends all the products returned by the database.
 * 
 * @param outFD File descriptor of OUT pipe where to write the products to.
 */
void send_products(int outFD) {
    Product *prods;
    int numProds = get_products(&prods);
    if (numProds < 0) {
        log_warn("DB: Error querying products from database.");
        int err = MESSAGE_ERROR;
        ensureWrite(&err, sizeof (err), outFD);
        return;
    }
    //Got products, now send em and free em at the same time
    ensureWrite(&numProds, sizeof (numProds), outFD);
    for (int i = 0; i < numProds; i++) {
        void* serialized;
        size_t serializedLen = serializeProduct(prods[i], &serialized);
        ensureWrite(&serializedLen, sizeof (serializedLen), outFD);
        ensureWrite(serialized, serializedLen, outFD);
        free(serialized);
    }
    for (int i = 0; i < numProds; i++) {
        free(prods[i]);
    }
    free(prods);
}

int get_stock(int productId) {
    int result;
    char *baseQuery = "SELECT quantity FROM products WHERE id=";
    char query[strlen(baseQuery) + countDigits(productId) + 1];
    char *err;
    snprintf(query, sizeof (query), "%s%i", baseQuery, productId);
    if (run_query(databaseHandle, query, (databaseCallback) store_single_number, &result, &err) != SQLITE_OK) {
        sqlite3_free(err);
        return -1;
    }
    return result;
}

/**
 * Inserts the specified <b>verified</b> order into the database. The order is
 * verified if:
 *  
 * - There is enough stock for every requested item
 * - The order has a non-NULL address
 * 
 * @param o The verified order.
 * @return The new order ID on success, -1 on error.
 */
sqlite3_int64 store_verified_order(Order o) {
    char *err;
    //Insert order
    time_t timestamp = time(NULL);
    char query[1024];
    int index = 0;
    char *baseQuery = "INSERT INTO orders (total, address, `time`) VALUES (";
    sprintf(query, "%s", baseQuery);
    index += strlen(baseQuery);
    sprintf(query + index, "%.2f,", order_get_total(o));
    index += countDigits((int) order_get_total(o)) + 1 + 2 + 1; //int digits + . + 2 decimal places + ,
    sprintf(query + index, "\"%s\",", order_get_addr(o));
    index += 1 + strlen(order_get_addr(o)) + 1 + 1; //" + address + " + ,
    sprintf(query + index, "%li)", timestamp); //TODO not portable
    index += countDigits(timestamp) + 1; //time + )
    query[index] = 0;
    if (run_query(databaseHandle, query, NULL, NULL, &err) != SQLITE_OK) {
        log_warn("Couldn't store verified order:");
        log_warn(err);
        sqlite3_free(err);
        return -1;
    }
    sqlite3_int64 orderID = sqlite3_last_insert_rowid(databaseHandle);
    //Now insert all the order's entries
    baseQuery = "INSERT INTO orderEntry (order_id, product_id, quantity) VALUES (";
    for (int i = 0; i < order_get_num_entries(o); i++) {
        OrderEntry e = order_get_entry(o, i);
        index = 0;
        sprintf(query, "%s", baseQuery);
        index += strlen(baseQuery);
        sprintf(query + index, "%lli,", orderID); //TODO not portable
        index += countDigits(orderID) + 1; //orderID + ,
        sprintf(query + index, "%i,", orderentry_get_id(e));
        index += countDigits(orderentry_get_id(e)) + 1; //productID + ,
        sprintf(query + index, "%i)", orderentry_get_quantity(e));
        index += countDigits(orderentry_get_quantity(e)) + 1; //productQuantity + )
        query[index] = 0;
        if (run_query(databaseHandle, query, NULL, NULL, &err) != SQLITE_OK) {
            log_warn("Couldn't store verified order entry:");
            log_warn(err);
            sqlite3_free(err);
            return -1;
        }
    }
    return orderID;
}

/**
 * Updates the stock in the database by removing all the items in the specified
 * <b>verified</b> order. The order is
 * verified if:
 *  
 * - There is enough stock for every requested item
 * - The order has a non-NULL address
 * 
 * @param o The verified order.
 * @return 1 on success, -1 on error.
 */
int update_stock(Order o) {
    char *err;
    char query[1024];
    for (int i = 0; i < order_get_num_entries(o); i++) {
        OrderEntry e = order_get_entry(o, i);
        sprintf(query, "UPDATE products SET quantity=quantity-%i WHERE id=%i", orderentry_get_quantity(e), orderentry_get_id(e));
        if (run_query(databaseHandle, query, NULL, NULL, &err) != SQLITE_OK) {
            log_warn(err);
            sqlite3_free(err);
            return -1;
        }
    }
    return 1;
}

/**
 * Verifies that there is enough stock to meet the specified order. If there is
 * not enough stock for any item in the order, its order entry is updated to
 * the current stock.
 * 
 * @param o The order to verify. <b>Note: </b> Will be modified if unsatisfiable.
 * @return 1 if the order was verified, 0 otherwise. In this case, order will be
 * modified to represent the largest subset of the order that can be satisfied.
 */
int verifyOrderStock(Order *o) {
    int satisfied = 1;
    //Verify that every item quantity has enough stock
    for (int i = 0; i < order_get_num_entries(o); i++) {
        OrderEntry e = order_get_entry(o, i);
        int quantity = orderentry_get_quantity(e),
                stock = get_stock(orderentry_get_id(e));
        if (stock < quantity) {
            satisfied = 0;
            orderentry_set_quantity(e, stock);
        }
    }
    return satisfied;
}

/**
 * Validates a received order. If the order can't be satisfied, sends the
 * biggest sub-order that can be satisfied at the time.
 */
void place_order(int inFD, int outFD) {
    size_t serializedSize;
    ensureRead(&serializedSize, sizeof (serializedSize), inFD);
    void *serialized = malloc(serializedSize);
    ensureRead(serialized, serializedSize, inFD);
    Order order = order_unserialize(serialized);
    free(serialized);
    int satisfiable = verifyOrderStock(&order);
    int responseCode;
    if (satisfiable) {
        if (store_verified_order(order) == -1 || update_stock(order) == -1) {
            responseCode = MESSAGE_ERROR;
            log_warn("Database error verifying order.");
        } else {
            responseCode = MESSAGE_OK;
            log_info("Order placed successfully.");
        }
        ensureWrite(&responseCode, sizeof (responseCode), outFD);
    } else { //Couldn't satisfy, send largest satisfiable sub-order
        responseCode = MESSAGE_UNSATISFIABLE_ORDER;
        ensureWrite(&responseCode, sizeof (responseCode), outFD);
        serializedSize = order_serialize(order, (void **) &serialized);
        ensureWrite(&serializedSize, sizeof (serializedSize), outFD);
        ensureWrite(serialized, serializedSize, outFD);
        log_info("Received unsatisfiable order, replied with biggest satisfiable sub-order.");
        free(serialized);
    }
}

/**
 * Initializes the database with default values.
 * 
 * @return 1 if there were no errors with any of the queries, 0 otherwise. Note
 * that 0 is not fatal; some records might already exist, for example.
 */
int init() {
    int result = 1;
    char* queries[] = {"CREATE TABLE IF NOT EXISTS products(id INTEGER PRIMARY KEY NOT NULL, name TEXT NOT NULL, quantity INTEGER NOT NULL DEFAULT 0 check(quantity >= 0), price FLOAT NOT NULL, description TEXT);",
        "CREATE TABLE IF NOT EXISTS orders(id INTEGER PRIMARY KEY NOT NULL, total FLOAT NOT NULL, address TEXT, `time` INTEGER NOT NULL);",
        "CREATE TABLE IF NOT EXISTS orderEntry(id INTEGER PRIMARY KEY NOT NULL, order_id INTEGER NOT NULL, product_id INTEGER NOT NULL, quantity INTEGER NOT NULL DEFAULT 1, FOREIGN KEY (order_id) REFERENCES orders(id), FOREIGN KEY (product_id) REFERENCES products(id));",
        "INSERT INTO products VALUES(1, \"Vodka\", 60, 30.5, \"A Russian favourite imported from the heart of the coldest place on Earth, Serbia\");",
        "INSERT INTO products VALUES(2, \"Gin\", 30, 40, \"A Dutch favourite\");",
        "INSERT INTO products VALUES(3, \"Chianti\", 40, 90, \"Aged wine imported from the cuore of Tuscany, Italy\");",
        "INSERT INTO products VALUES(4, \"Fernet\", 100, 25, \"Want to get drunk at a pregame with your amigos? Nothing beats the terrible hangover that Fernet gives you\");",
        "INSERT INTO products VALUES(5, \"Quilmes\", 200, 20, \"You aren't Argentine until you've drunk Quilmes, the most famous beer in la Tierra de la Plata\");"};
    char* err;
    for (int i = 0; i < sizeof (queries) / sizeof (queries[0]); i++) {
        if (run_query(databaseHandle, queries[i], NULL, NULL, &err) != SQLITE_OK) {
            log_warn(err);
            sqlite3_free(err);
            result = 0;
        }
    }
    return result;
}

void shut_down(int outFD, int inFD) {
    close(outFD);
    close(inFD);
    if (sqlite3_close(databaseHandle) != SQLITE_OK) {
        log_warn("Couldn't close database handle. Shutting down database server anyway.");
    }
    log_info("Bye-bye from database server.");
}

/* *****************************************************************************
 *                                  MAIN
 * ****************************************************************************/

/**
 * Main for database server. This should be called with an <i>execve()</i> after
 * a parent process creates a shared database connection and forks. Then this
 * should be called with the following argv parameters in this order:
 * 
 * 1) Filename of this executable (http://man7.org/linux/man-pages/man2/execve.2.html)
 * 2) FD from which the server will READ
 * 3) FD from which the server will WRITE
 * 4) NULL
 * 
 * @param argc Should be 3, always.
 * @param argv Array of parameters as described above.
 * @param envp Unused.
 * @return 0 on success, nonzero on error.
 */
int main(int argc, char *argv[], char *envp[]) {
    if (argc != 3) {
        log_err("Database started with invalid parameters. Aborting.");
        return -1;
    }
    int inFD = atoi(argv[1]),
            outFD = atoi(argv[2]),
            inSelect[1] = {inFD};
    if (sqlite3_open(dbFileName, &databaseHandle)) {
        log_err("Database server couldn't open database:");
        log_err(sqlite3_errmsg(databaseHandle));
        log_err("Database server aborting.");
        return 0;
    }
    if (!init()) {
        log_warn("Database initialized with errors.");
    }
    //Main loop
    log_info("This is database server, waiting for requests...");
    int done = 0;
    do {
        void* clientData = NULL;
        int s = select_wrapper(inFD + 1, inSelect, NULL, NULL, -1, -1);
        if (s == -1) {
            log_warn("Database select failed. Trying again.");
            continue;
        }
        //Received command code
        int msgCode;
        ensureRead(&msgCode, sizeof (msgCode), inFD);
        switch (msgCode) {
            case CMD_GET_PRODUCTS:
                log_info("Database server sending back products");
                send_products(outFD);
                break;
            case CMD_PLACE_ORDER:
                log_info("Database server placing order");
                place_order(inFD, outFD);
                break;
            case MESSAGE_CLOSE:
                log_info("Database received CLOSE command, shutting down.");
                done = 1;
                break;
            default:
                log_warn("WAT. Database received unknown message code. Behavior from here on is unspecified.");
                break;
        }
        if (clientData != NULL) {
            free(clientData);
        }
    } while (!done);
    shut_down(outFD, inFD);
    return 0;
}