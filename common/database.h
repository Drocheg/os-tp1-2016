/* 
 * File:   database.h
 * Author: juan_
 *
 * Created on April 13, 2016, 10:07 AM
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "../SQLite/src/sqlite3.h"

sqlite3 *connectToDB();

int disconnectFromDB(sqlite3 *connection);

#endif /* DATABASE_H */

