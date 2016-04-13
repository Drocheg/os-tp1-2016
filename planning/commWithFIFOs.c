#include "comm.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

struct connection_t {
    char *path;
    int clientPID;
    int serverPID;
};



/*
 * The general idea for OPENING a connection is as follows:
 * 1) Client creates FIFO - done
 * 2) Client writes its PID into the main server's FIFO (path defined in config file)
 * 2) Main server reads client PID and forks
 * 3) Child server forks, child process returns its PID to complete connection struct
 * 4) Client and Server can now communicate through FIFO created by Client.
 */
Connection openConnection(Server server) {
    
    Connection connection = malloc(sizeof(*connection));
    char path[16];
    int pid = getpid();
    sprintf(path, "/tmp/%i", pid);
    //0666 == anybody can read and write
    if (mkfifo(path, 0666)) {    //Error
        fprintf(stderr, "Error creating client FIFO.");
    }
    printf("Successfully created FIFO at %s", path);
    connection->path = malloc(strlen(path)+1);
    strcpy(connection->path, path);
    connection->clientPID = pid;
    return connection;
}

/*
 * The general idea for CLOSING a connection is as follows:
 * 1) Client/Server writes "CLOSE" on shared FIFO. This means Client/Server will
 * no longer write to/read from said FIFO. - done
 * 2) Client/Server calls remove(fifopath) - done
 * 3) When done, Server/Client and any other processes using the FIFO should
 * also call remove(fifopath) so as to actually remove the "file."
 */
int closeConnection(Connection connection) {
    
    FILE *f = fopen(connection->path, "w");
    char *msg = "CLOSE";
    /*
     * BLOCKING: This won't run unless there's another proces reading from the FIFO
     * If testing this, run cat < /path/to/fifo and you'll see the data and the program will close the connection
     */
    fwrite(&msg, strlen(msg)+1, 1, f);
    fclose(f);
    remove(connection->path);
    printf("Successfully closed FIFO %s", connection->path);
}



