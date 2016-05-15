#include "database.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <product.h>

typedef int (*databaseCallback)(void*, int, char**, char**);

const char* dbFileName = "db";
sqlite3 *databaseHandle;

int printRow(void *unused, int numCols, char **colData, char **colName) {
    int i;
    for(i=0; i < numCols; i++) {
        printf("%s = %s\n", colName[i], colData[i] ? colData[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int run_query(sqlite3* connection, const char* query, databaseCallback callback, void* firstCallbackArgument, char **errorMsgDest) {
    return sqlite3_exec(connection, query, callback, firstCallbackArgument, errorMsgDest);
}

int main() {
    if(sqlite3_open(dbFileName, &databaseHandle) != SQLITE_OK) {
        printf("Couldn't open database handle. Aborting.\n");
        return -1;
    }
    char* errorMsg = NULL;    
    char query[1024];
    while(1) {
        printf("Enter query or q to exit: ");
        int i = 0, c;
        while(i < sizeof(query)-1 && (c = getchar()) != '\n') {
            query[i++] = c;
        }
        query[i] = 0;
        if(strcmp(query, "q") == 0) {
            break;
        }
        printf("Executing \"%s\":\n", query);
        if(run_query(databaseHandle, query, printRow, NULL, &errorMsg) == SQLITE_OK) {
            printf("Done.\n");
        }
        else {
            printf("SQL error: %s\n", errorMsg);
            sqlite3_free(errorMsg);
        }
    }
    if (sqlite3_close(databaseHandle) != SQLITE_OK) {
        printf("Couldn't close database handle. Shutting down anyway.\n");
    }
    return 0;
}