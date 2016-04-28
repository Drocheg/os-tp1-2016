#include "database.h"
#include <stdio.h>

int db_run_query(sqlite3* connection, const char* query, databaseCallback callback, void* firstCallbackArgument, char **errorMsgDest) {
    return sqlite3_exec(connection, query, callback, firstCallbackArgument, errorMsgDest);
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