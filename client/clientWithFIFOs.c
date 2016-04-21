/* 
 * File:   main.c
 * Author: juan_
 *
 * Created on April 12, 2016, 9:54 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //For sleep
#include <config.h>
#include <comm.h>
#include <database.h>

int main(int argc, char** argv) {
//    if(!fork()) {
//        listenToClients();
//        return 0;
//    }
    
    
    /*
     * Connection test - when running, open a console and CD into /tmp
     * When the FIFOs are created, run
     * cat (pid)-out to read first message
     * cat (pid)-out again to read CLOSE message
     */
    //    printf("Server config FIFO: %s", getServerAddress());
    Connection c = conn_open(getServerAddress());
    fflush(stdout);
    conn_send(c, "Hello, server!", 15);
    sleep(5);
    conn_close(c);
    fflush(stdout);
    remove(getServerAddress());
    
    //    sqlite3 *db = connectToDB();
    //    printf("Successfully connected to database.\n");
    //    printf("Disconnecting from DB returned %i\n", disconnectFromDB(db));
    
    return 0;
}