#include "database.h"
#include <stdio.h>
#include <stdlib.h>

static int rowCounter(int *result, int numCols, char **colData, char **colName) {
    *result = strtol(colData[0], NULL, 10);
    return 0;
}

static int productLoader(Product *array, int numCols, char **colData, char **colName) {
    int i = 0;
    //TODO don't go to end of array on every item
    while(array[i] != NULL) //Append to end of array
        i++;
    array[i] = newProduct(colData[0], colData[1], atof(colData[2]), atoi(colData[3]));
    return 0;
}

int db_run_query(sqlite3* connection, const char* query, databaseCallback callback, void* firstCallbackArgument, char **errorMsgDest) {
    return sqlite3_exec(connection, query, callback, firstCallbackArgument, errorMsgDest);
}

int db_get_products(sqlite3* connection, Product **destArray) {
    char *errormsg;
    int numProducts;
    db_run_query(connection, "SELECT count(id) FROM products", (databaseCallback)rowCounter, &numProducts, &errormsg);
    if(numProducts <= 0) {
        return -1;
    }
    *destArray = calloc(numProducts, sizeof(*destArray));
    db_run_query(connection, "SELECT name, description, price, quantity FROM products", (databaseCallback)productLoader, *destArray, &errormsg);
    return numProducts;
}

int db_place_order(sqlite3* connection, Order order) {
    
}

sqlite3 *db_connect() {
    sqlite3 *result;
    int number = sqlite3_open("db", &result); //Create temporary database
    if(number) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(result));
        return NULL;
    }
    return result;
}

int db_disconnect(sqlite3 *connection) {
    return sqlite3_close(connection);
}