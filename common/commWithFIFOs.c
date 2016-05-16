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
#include <sys/select.h>
#include <sys/file.h>

struct connection_t {
    char* outFIFOPath;
    char* inFIFOPath;
    int outFD;
    int inFD;
};

struct conn_params_t {
    int connRequestsFD;   // File descriptor of open file where to listen for connection requests
};

/**
 * Creates two FIFOs for a process to communicate with a different process, and
 * stores their paths in the specified connection. Does not open the FIFOs.
 * The created FIFOs have the names:
 * (basePath)-in For incoming data
 * (basePath)-out For outgoing data
 * 
 * @param const char* basePath The base pathname of the FIFOs to create.
 * @param Connection c The connection where to store the paths of the created
 * FIFOs.
 * @return int 1 on success, 0 on failure.
 */
static int createProcessFIFOs(const char* basePath, Connection c);

/**
 * Sends the paths of two FIFOs previously created with <i>createProcessFIFOs()</i>
 * to the main server using the specified FIFO path.
 * 
 * @param Connection c The connection whose FIFO paths to send.
 * @param const char* mainServerFIFO The path of the main server FIFO.
 * @return int 1 on success, 0 on failure.
 */
static int sendFIFOPaths(Connection c, const char* mainServerFIFO);

/*
 * Creates a connection between the current process and a server.
 * The procedure is as follows:
 *
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
    
    //Step 1
    pid_t pid = getpid();
    char buff[5+countDigits(pid)+1];    // "/tmp/" + pid + \0
    sprintf(buff, "/tmp/%i", pid);
    if(!createProcessFIFOs(buff, connection)) {
        //printf("Couldn't create connection FIFOs for process #%i\n", pid);
        return NULL;
    }
    
    //Step 2
    connection->inFD = open(connection->inFIFOPath, O_RDONLY|O_NONBLOCK);
    fcntl(connection->inFD, F_SETFL, fcntl(connection->inFD, F_GETFL)&~O_NONBLOCK);	//Remove nonblocking flag
    
    //Step 3
    if(!sendFIFOPaths(connection, address)) {
        //printf("Couldn't send FIFO information to main server for process #%i\n", pid);
        return NULL;
    }
    
    //Wait for OK (steps 4-5)
    /*
     * Avoid race condition (reading from FIFO while the server is opening the
     * FIFO, conn_receive will fail until it's opened for writing by the server)
     * with select()
     */
    int fds[1] = {connection->inFD};
    int selectResult = select_wrapper(connection->inFD+1, fds, NULL, NULL, 10, 0);
    if(selectResult == -1) {  //Error
        //printf("Error with select(). Aborting.\n");
        return NULL;
    }
    else if(selectResult == 0) {    //Timed out
        //printf("Timed out. Aborting.\n");
        return NULL;
    }
    //Else, FIFO is open and with data
    char *ack;
    if(conn_receive(connection, (void **)&ack, NULL) == 0) {
        //printf("conn_receive failed. Aborting.\n");
        return NULL;
    }
    int code = *(int *)(ack);
    //Step 6
    if(code == MESSAGE_OK) {	//Server forked and listening, open write FIFO again
    	connection->outFD = open(connection->outFIFOPath, O_WRONLY);
    	return connection;
    }
    else {
    	//printf("Error, received something else: %i\nAborting.\n", code);
    	return NULL;
    }
}

int conn_close(Connection conn) {
    close(conn->outFD);
    close(conn->inFD);
    remove(conn->outFIFOPath);		//Removes the file
    remove(conn->inFIFOPath);
    free(conn->outFIFOPath);
    free(conn->inFIFOPath);
    free(conn);
    return 1;
}

int conn_send(const Connection conn, const void* data, const size_t length) {
    if(length == 0) {
        return 1;
    }
    return ensureWrite(&length, sizeof(length), conn->outFD)
            && ensureWrite(data, length, conn->outFD);
}

int conn_receive(const Connection conn, void** data, size_t* length) {
    size_t lenAux;
    if (length == NULL) { //If there's nowhere to store the read length, store it within the function, then discard
        length = &lenAux;
    }
    if(!ensureRead(length, sizeof(*length), conn->inFD)) {
        return 0;
    }
    *data = malloc(*length);
    if(!ensureRead(*data, *length, conn->inFD)) {
        if(*data != NULL) {
            free(*data);
        }
        return 0;
    }
    return 1;
}

ConnectionParams conn_listen(char *listeningAddress) {
    //Create FIFO where process will listen for connection requests
    if(mkfifo(listeningAddress, 0666) == -1) {
        //printf("Error creating connection FIFOs for process #%i.\n", getpid());
        return NULL;
    }
    ConnectionParams p = malloc(sizeof(*p));
    int fd = open(listeningAddress, O_RDONLY|O_NONBLOCK);           // Open in non-blocking read mode to prevent blocking, but remove flag immediately after
    if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)&~O_NONBLOCK) == -1) {  // Remove nonblocking flag, we want reads to be blocking later
        //printf("Couldn't open requests file in non-blocking mode for process #%i\n", getpid());
        return NULL;
    }
    p->connRequestsFD = fd;
    return p;
}

Connection conn_accept(ConnectionParams params) {
    int fds[1] = {params->connRequestsFD};
    int selectResult = select_wrapper(params->connRequestsFD+1, fds, NULL, NULL, -1, -1);
    if (selectResult == -1) {  //Error
        //printf("Error with select(). Aborting.\n");
        return NULL;
    }
    else {
        Connection c = malloc(sizeof(*c));
        /*
         * Read connection info as follows:
         * 1) Out pipe path length
         * 2) Out pipe as IN pipe
         * 3) In pipe path length
         * 4) In pipe as OUT pipe
         */
        size_t len = -1;
        if(!ensureRead(&len, sizeof(len), params->connRequestsFD))
            return NULL;
        c->inFIFOPath = malloc(len);
        if(!ensureRead(c->inFIFOPath, len, params->connRequestsFD))
            return NULL;
        if(!ensureRead(&len, sizeof(len), params->connRequestsFD))
            return NULL;
        c->outFIFOPath = malloc(len);
        if(!ensureRead(c->outFIFOPath, len, params->connRequestsFD))
            return NULL;
        //Open the FIFOs
        int inFD = open(c->inFIFOPath, O_RDONLY|O_NONBLOCK);
        fcntl(inFD, F_SETFL, fcntl(inFD, F_GETFL)&~O_NONBLOCK); //Remove nonblocking flag
        c->inFD = inFD;
        int outFD = open(c->outFIFOPath, O_WRONLY); //This should not fail, client has opened in read mode
        c->outFD = outFD;
        //Connection complete, send OK to other end
        int ok = MESSAGE_OK;
        conn_send(c, &ok, sizeof(ok));
        return c;
    }
}

static int createProcessFIFOs(const char* basePath, Connection c) {
    int baseLength = strlen(basePath);
    //Make out FIFO
    if((c->outFIFOPath = malloc(baseLength+4+1)) == NULL) { //path+"-out"+\0
        return 0;
    }
    sprintf(c->outFIFOPath, "%s-out", basePath);
    if(mkfifo(c->outFIFOPath, 0666) == -1) {
        //If this fails, it might be because there are leftover FIFOs that weren't erased and the file already exists
        free(c->outFIFOPath);
        return 0;
    }
    //Make in FIFO
    if((c->inFIFOPath = malloc(baseLength+3+1)) == NULL) {  //path+"-in"+\0
        return 0;
    }
    sprintf(c->inFIFOPath, "%s-in", basePath);
    if(mkfifo(c->inFIFOPath, 0666) == -1) {
        //If this fails, it might be because there are leftover FIFOs that weren't erased and the file already exists
        free(c->inFIFOPath);
        return 0;
    }
    return 1;
}

static int sendFIFOPaths(Connection c, const char* mainServerFIFO) {
    int fd = open(mainServerFIFO, O_WRONLY|O_NONBLOCK);    //This will block if the main server isn't reading at the address
    if(fd == -1) {
        return 0;
    }
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL)&~O_NONBLOCK);     //Remove nonblocking flag
    int lock = flock(fd, LOCK_EX);
    if(lock != 0) { //Couldn't obtain lock for some bizarre reason
        return 0;
    }
    size_t len = strlen(c->outFIFOPath)+1;
    if(!ensureWrite(&len, sizeof(len), fd) || !ensureWrite(c->outFIFOPath, len, fd)) {
        close(fd);
        return 0;
    }
    len = strlen(c->inFIFOPath)+1;
    if(!ensureWrite(&len, sizeof(len), fd) || !ensureWrite(c->inFIFOPath, len, fd)) {
        close(fd);
        return 0;
    }
    flock(fd, LOCK_UN);
    close(fd);
    return 1;
}