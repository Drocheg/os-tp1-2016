#include "database.h"
#include <stdio.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

sqlite3 *connectToDB() {
    sqlite3 *result;
    int number;
    number = sqlite3_open("", &result); //Create temporary database
    if(number) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(result));
        sqlite3_close(result);
        return NULL;
    }
//    rc = sqlite3_exec(result, argv[2], callback, 0, &zErrMsg);
//    if( rc!=SQLITE_OK ){
//        fprintf(stderr, "SQL error: %s\n", zErrMsg);
//        sqlite3_free(zErrMsg);
//    }
//    sqlite3_close(result);
    return result;
}

int disconnectFromDB(sqlite3 *connection)// </editor-fold>
 {
    return sqlite3_close(connection);
}