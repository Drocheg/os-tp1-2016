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
#include <string.h>

int main(int argc, char** argv) {
    /*
     * Connection test - when running, open a console and CD into /tmp
     * When the FIFOs are created, run
     * cat (pid)-out to read first message
     * cat (pid)-out again to read CLOSE message
     */
    //    FILE *f = fopen(getServerAddress(), "w");
    //    if(f == NULL) {
    //        fprintf(stderr, "Couldn't open server FIFO.\n");
    //        return -1;
    //    }
    //    char *str = "Hello!";
    //    int len = strlen(str)+1;
    //    int written = 0;
    //    while(written < sizeof(len)) {
    //        written += write(fileno(f), (&len)+written, sizeof(len)-written);
    //    }
    //    written = 0;
    //    while(written < sizeof(len)) {
    //        written += write(fileno(f), str+written, len-written);
    //    }
    //    printf("Successfully wrote to server.\n");
    //    return 0;
    
    
    printf("Connecting to server via %s\n", getServerAddress());
    fflush(stdout);
    Connection c = conn_open(getServerAddress());
    fflush(stdout);
    conn_send(c, "Hello, server!", 15);
    conn_close(c);
    fflush(stdout);
    
    //    sqlite3 *db = connectToDB();
    //    printf("Successfully connected to database.\n");
    //    printf("Disconnecting from DB returned %i\n", disconnectFromDB(db));
    return 0;
}
