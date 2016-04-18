#include "comm.h"
#include "config.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

struct connection_t {
    char* outFIFOPath;
    char* inFIFOPath;
};

/*
 * The general idea for opening a connection with FIFOs is as follows:
 * 1) Client creates FIFO connection with the server (path defined in config file).
 * 2) Client creates 2 FIFOs and sends the two file paths to the server:
 *      2.1) (pid)-out for sending messages (outgoing for this process)
 *      2.2) (pid)-in for receiving messages (incoming for this process)
 *      NOTE: (pid) is the current process' PID. For now this means every
 *      process can only have 1 open connection to the server, otherwise
 *      different servers will start breaking the FIFOs.
 * 3) Server opens the 2 FIFOs and forks, setting up the FIFOs appropriately (-out is in for reading, -in is out for writing)
 * 4) Client also opens the 2 FIFOs, setting them up appropriately (-out is in for writing, -in is out for reading)
 * 5) Voilà!
 */


/*
 * Creates a connection between the current process and a server.
 */
Connection conn_open(char* address) {
    Connection connection = malloc(sizeof(*connection));
    //TODO make this variable, could be 64-bit int, use sizeof(pid_t)
    char pid[11];   //Max 32-bit int size = 10 + 1 for null terminator
    sprintf(pid, "%i", getpid());
    int pathLength = 5+strlen(pid); //"/tmp/"+pid
    connection->outFIFOPath = malloc(pathLength+4+1);   //path+"-out"+\0
    connection->inFIFOPath = malloc(pathLength+3+1);    //path+"-in"+\0
    sprintf(connection->outFIFOPath, "/tmp/%s-out", pid);
    sprintf(connection->inFIFOPath, "/tmp/%s-in", pid);
    //0666 == anybody can read and write TODO change?
    if (mkfifo(connection->outFIFOPath, 0666)) {	//-1 = error
        fprintf(stderr, "Error creating outgoing FIFO.");
    }
    if (mkfifo(connection->inFIFOPath, 0666)) {	//Error
        fprintf(stderr, "Error creating incoming FIFO.");
    }
    printf("Successfully created FIFOs for process with PID %s\n", pid);
    //MALLOCS hacer asi
    //	connection->path = malloc(strlen(path)+1);
    //	strcpy(connection->path, path);
    //	connection->clientPID = pid;
    //Write FIFO paths on main server FIFO
    FILE *f = fopen(address, "a");
    int l = strlen(connection->outFIFOPath);
    fwrite(&l, sizeof(l), 1, f);
    fwrite(connection->outFIFOPath, l, 1, f);
    l = strlen(connection->inFIFOPath);
    fwrite(&l, sizeof(l), 1, f);
    fwrite(connection->inFIFOPath, l, 1, f);
    printf("Successfully wrote connection info on server for PID %s.\n", pid);
    return connection;
}

/*
 * Closes a connection (i.e. finishes connection)
 */
int conn_close(Connection connection) {
    char *msg = CLOSE_MESSAGE;
    /*
     * BLOCKING: This won't run unless there's another process reading from the FIFO
     * If testing this, run:
     * cat /path/to/fifo
     * and you'll see the data and the program will close the connection
     */
    conn_send(connection, msg, strlen(msg)+1);
    remove(connection->outFIFOPath);		//Removes the file
    remove(connection->inFIFOPath);
    printf("Successfully closed FIFOs %s and %s", connection->inFIFOPath, connection->outFIFOPath);
    free(connection->outFIFOPath);
    free(connection->inFIFOPath);
    free(connection);
    return 1;
}

/*
 * Sends the specified message to the other endpoint of the specified connection. Asynchronous. To expect a response, call receive() afterwards.
 * 1) Sends message length (4 bytes little-endian integer)
 * 2) Sends message data (of the specified length)
 * Returns 0 on error or some other number on success.
 */
int conn_send(Connection connection, char* data, int length) {
    printf("Sending %i bytes to %s...", length, connection->outFIFOPath);
    fflush(stdout);
    FILE *f = fopen(connection->outFIFOPath, "a");
    //TODO Fwrite puede no haber escrito todo, chequear
    fwrite(&length, sizeof(length), 1, f);	//Write length
    fwrite(data, length, 1, f);	//Write data
    fclose(f);
    printf("Done.\n");
    fflush(stdout);
    return 1;
}

/*
 * Awaits to receive a message from the other endpoint.
 */
int conn_receive(Connection conn, char** data, int* length) {
    int result = 0;
    FILE *f = fopen(conn->inFIFOPath, "r");
    //TODO Fread puede no haber escrito todo, chequear
    printf("Waiting for data at %s...", conn->inFIFOPath);
    fflush(stdout);
    fread(length, sizeof(*length), 1, f);
    *data = malloc(*length);
    fread(*data, *length, 1, f);
    fclose(f);
    printf("read %i bytes.\n", *length);
    fflush(stdout);
    return result;
}