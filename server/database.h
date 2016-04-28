/* 
 * File:   database.h
 * Author: juan_
 *
 * Created on April 13, 2016, 10:07 AM
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "../SQLite/src/sqlite3.h"

typedef int (*databaseCallback)(void*, int, char**, char**);

sqlite3 *db_connect();

int db_run_query(sqlite3* connection, const char* query, databaseCallback callback, void* firstCallbackArgument, char **errorMsgDest);

int db_disconnect(sqlite3 *connection);

#endif /* DATABASE_H */

