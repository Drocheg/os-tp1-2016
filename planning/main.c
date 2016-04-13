/* 
 * File:   main.c
 * Author: juan_
 *
 * Created on April 12, 2016, 9:54 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "comm.h"
#include "database.h"

int main(int argc, char** argv) {
//    printf("Server config FIFO: %s", getServerAddress());
//    Connection c = openConnection(NULL);
//    fflush(stdout);
//    sleep(5);
//    closeConnection(c);
//    fflush(stdout);
//    sleep(5);
    
    sqlite3 *db = connectToDB();
    printf("Successfully connected to database.\n");
    printf("Disconnecting from DB returned %i\n", disconnectFromDB(db));
    return 0;
    
    return 0;
}

