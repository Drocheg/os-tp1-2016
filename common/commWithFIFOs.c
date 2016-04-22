#include <comm.h>
#include <lib.h>
#include <config.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

/*
 * Creates a connection between the current process and a server.
 * The procedure is as follows:
 * 1) Client creates FIFO for reading (pid-in) and writing (pid-out),
 * where (pid) is the current process ID.
 * 2) Client opens pid-in for reading non-blockingly. Does NOT
 * open pid-out.
 * 3) Client sends the two FIFO paths to the main server (address
 * defined in config file):
 *      3.1) (pid)-out
 *      3.2) (pid)-in
 *      NOTE: For now this means every process can only have 1 open
 *      connection to the server, otherwise different servers will
 *      start stepping on each other's feet.
 * 4) Main server forks and reads FIFO information backwards
 * (pid-out is in for server, pid-in is out).
 * 5) Forked server opens (pid)-in for writing and (pid)-out for reading, and
 * sends ACK message to client.
 * 6) Client receives ACK, opens (pid)-out for writing (if server hadn't opened
 * it for reading, it would block or fail if opening non-block)
 * 7) Voilà! Client and server can communicate.
 */
Connection conn_open(const char* address) {
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
    connection->inFD = open(connection->inFIFOPath, O_RDONLY|O_NONBLOCK);
    int flagThing = fcntl(connection->inFD, F_SETFL, fcntl(connection->inFD, F_GETFL)&~O_NONBLOCK);	//Remove nonblocking flag
    connection->outFD = open(connection->outFIFOPath, O_WRONLY|O_NONBLOCK);	//This will probably fail TODO remove
    flagThing = fcntl(connection->outFD, F_SETFL, fcntl(connection->outFD, F_GETFL)&~O_NONBLOCK);	//Remove nonblocking flag
    printf("Successfully created FIFOs for process with PID %s\n", pid);
    //Write FIFO paths on main server FIFO
    int fd = open(address, O_WRONLY);
    size_t len = strlen(connection->outFIFOPath)+1;
    ensureWrite(&len, sizeof(len), fd);
    ensureWrite(connection->outFIFOPath, len, fd);
    len = strlen(connection->inFIFOPath)+1;
    ensureWrite(&len, sizeof(len), fd);
    ensureWrite(connection->inFIFOPath, len, fd);
    close(fd);
    printf("Successfully wrote connection info on server for PID %s. Waiting for OK...", pid);
    fflush(stdout);
    char *ack;
    len = strlen(MESSAGE_OK)+1;
    conn_receive(connection, (void **)&ack, &len);
    if(strcmp(ack, MESSAGE_OK) == 0) {	//Server forked and listening, open write FIFO again
    	printf("received.\n");
    	connection->outFD = open(connection->outFIFOPath, O_WRONLY);
    }
    else {
    	printf("Error, received something else: %s\n", ack);
    }
    return connection;
}

/*
 * Closes a connection (i.e. finishes connection)
 */
int conn_close(Connection conn) {
	conn_send(conn, MESSAGE_CLOSE, strlen(MESSAGE_CLOSE)+1);
	close(conn->outFD);
	close(conn->inFD);
    remove(conn->outFIFOPath);		//Removes the file
    remove(conn->inFIFOPath);
    printf("Connection closed.\n");
    free(conn->outFIFOPath);
    free(conn->inFIFOPath);
    free(conn);
    return 1;
}

/*
 * Sends the specified message to the other endpoint of the specified connection. Asynchronous. To expect a response, call receive() afterwards.
 * 1) Sends message length (4 bytes little-endian integer)
 * 2) Sends message data (of the specified length)
 * Returns 0 on error or some other number on success.
 */
int conn_send(const Connection conn, const void* data, size_t length) {
//    printf("Sending %i bytes to %s...", length, conn->outFIFOPath);
//    fflush(stdout);
	ensureWrite(&length, sizeof(length), conn->outFD);
	ensureWrite(data, length, conn->outFD);
//    printf("Done.\n");
    return 1;
}

/*
 * Awaits to receive a message from the other endpoint.
 */
int conn_receive(const Connection conn, void** data, size_t* length) {
//    printf("Waiting for data at %s...", conn->inFIFOPath);
//    fflush(stdout);
	ensureRead(length, sizeof(*length), conn->inFD);
	*data = malloc(*length);
	ensureRead(*data, *length, conn->inFD);
//    printf("read %i bytes.\n", *length);
    return 1;
}
