#include "comm.h"
#include "lib.h"
#include "config.h"
#include "forkedServer.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>


#ifndef MAIN_SERVER_CONN
#define MAIN_SERVER_CONN 1
#endif

#ifndef FORKED_SERVER_CONN
#define FORKED_SERVER_CONN 2
#endif

#ifndef CLIENT_CONN
#define CLIENT_CONN 3
#endif


struct connection_t {
    char* mainServerPath;
    char* outFIFOPath;
    char* inFIFOPath;
    int outFD;
    int inFD;
};



/*
 * Convention: 
 *      "s" + filepath = main server
 *      "f" + filepath = forked server
 *      "c" + filepath = client
 */
static int checkConnection(const char* address) {

    switch(address[0]) {
        case 's': return MAIN_SERVER_CONN;
        case 'f': return FORKED_SERVER_CONN;
        case 'c': return CLIENT_CONN;
    }
    return 0;
}


static int countDigits(pid_t pid) {

    int i = (pid <= 0) ? 1 : 0;
    
    while(pid != 0) {
        pid /= 10;
        i++;
    }
    return i;
}


static int createMainServerFIFO(Connection connection) {

    char* address = connection->mainServerPath;
    connection->inFIFOPath = malloc(strlen(address) + 1);
    connection->outFIFOPath = NULL;

    if(mkfifo(address, 0666)) { 
        if(remove(address) || mkfifo(address, 0666)) {
            fprintf(stderr, "Error creating main server FIFO.");
            return -1;
        }
    }
    return 0;
}

/*
 * Client creates FIFOs for reading (pid-in) and writing (pid-out)
 */
static int createFIFOs(Connection connection) {

    pid_t pid = getpid();
    char* pidStr = calloc(1, countDigits(pid) + 1);
    int pathLength = 5 + strlen(pidStr); /* "/tmp/" + pidStr */
    
    sprintf(pidStr, "%i", pid);
    connection->outFIFOPath = malloc(pathLength+4+1); /* path + "-out" +\0 */
    connection->inFIFOPath = malloc(pathLength+3+1); /*path + "-in" + \0 */
    sprintf(connection->outFIFOPath, "/tmp/%s-out", pidStr);
    sprintf(connection->inFIFOPath, "/tmp/%s-in", pidStr);

    /* 0666 == anybody can read and write */
    if (mkfifo(connection->outFIFOPath, 0666) || mkfifo(connection->inFIFOPath, 0666)) {
        fprintf(stderr, "Error creating connection FIFOs.\n");
        return -1;
    }
    return 0;
}

static int checkFlag(int flag) {

    if (flag == -1) {
        fprintf(stderr, "Error manipulating flags.\n");
        return -1;
    }
    return 0;
}


static int openMainServerFD(Connection connection) {

    int flag;
    connection->inFD = open(connection->mainServerPath, O_RDWR|O_NONBLOCK);
    connection->outFD = -1;
    
    if(connection->inFD < 0) {
        fprintf(stderr, "Couldn't open main server FIFO. Aborting.\n");
        return -1;
    }
    flag = fcntl(connection->inFD, F_GETFL);
    if (checkFlag(flag)) {
        return -1;
    }
    flag = fcntl(connection->inFD, F_SETFL, flag&~O_NONBLOCK);
    if (flag) {
        return -1;
    }
    return 0;
}

static int openInFD(int fd, char* path) {

    int flag;

    fd = open(path, O_RDONLY|O_NONBLOCK);
    if(fd < 0) {
        fprintf(stderr, "Couldn't open connection.\n");
        return -1;
    }
    flag = fcntl(fd, F_GETFL);
    if (flag == -1) {
        fprintf(stderr, "Couldn't open connection.\n");
        return -1;
    }
    flag = fcntl(fd, F_SETFL, flag&~O_NONBLOCK);
    if (flag == -1) {
        fprintf(stderr, "Couldn't open connection.\n");
        return -1;
    }
    return fd;

}

static int openOutFD(int fd, char* path) {

    int flag;

    fd = open(path, O_WRONLY);
    if(fd < 0) {
        fprintf(stderr, "Couldn't open connection.\n");
        return -1;
    }
    return fd;

}

static int openConnFD(Connection connection) {

    int flagThing;

    if ( (connection->inFD = openInFD(connection->inFD, connection->inFIFOPath)) < 0 ) {
        return -1;
    }
    if( (connection->outFD = openOutFD(connection->outFD, connection->outFIFOPath)) < 0) {
        return -1;
    }
    return 0;
}

static int checkFIFOWrites(int result) {

    if (result) {
        return 0;
    }
    fprintf(stderr, "Couldn't send FIFOs Paths.\n");
    return -1;
}

static int sendFIFOPaths(Connection connection) {

    int fd = open(connection->mainServerPath, O_WRONLY);
    size_t len = 0;
    
    if (fd < 0) {
        fprintf(stderr, "Couldn't send FIFOs Paths.\n");
        return -1;
    }
    len = strlen(connection->outFIFOPath) + 1;
    if (checkFIFOWrites(ensureWrite((void*)&len, sizeof(len), fd))) {
        return -1;
    }
    if (checkFIFOWrites(ensureWrite((void*)connection->outFIFOPath, len, fd))) {
        return -1;
    }
    len = strlen(connection->inFIFOPath) + 1;
    if (checkFIFOWrites(ensureWrite((void*)&len, sizeof(len), fd))) {
        return -1;
    }
    if (checkFIFOWrites(ensureWrite((void*)connection->inFIFOPath, len, fd))) {
        return -1;
    }
    if (close(fd) < 0) {
        return -1;
    }
    return 0;
}

void setup(Connection connection, fd_set* rfds, struct timeval* tv){

    FD_ZERO(rfds);
    FD_SET(connection->inFD, rfds);
    tv->tv_sec = 5;
    tv->tv_usec = 0;

} 



static int checkFIFOReads(int result) {

    if (result) {
        return 0;
    }
    fprintf(stderr, "Couldn't attend connection.\n");
    return -1;
}


static int attendConnection(Connection mainConn, Connection forkConn) {

    size_t length = 0;

    if (checkFIFOReads(ensureRead(&length, sizeof(size_t), mainConn->inFD))) {
        return -1;
    }
    forkConn->inFIFOPath = malloc(length);    
    if (checkFIFOReads(ensureRead(forkConn->inFIFOPath, length, mainConn->inFD))) {
        return -1;
    }

    if (checkFIFOReads(ensureRead(&length, sizeof(size_t), mainConn->inFD))) {
        return -1;
    }
    forkConn->outFIFOPath = malloc(length);
    if (checkFIFOReads(ensureRead(forkConn->outFIFOPath, length, mainConn->inFD))) {
        return -1;
    }

    return 0;
}

static Connection createForkedConnection(Connection mainConn) {

    Connection forkConn;
    char* address = malloc(strlen(mainConn->mainServerPath) + 2);
    address[0] = 'f';
    strcpy((char*)((void*)address + 1), mainConn->mainServerPath);
    printf("Addres es: %s\n", address);
    forkConn = conn_open(address);
    if (attendConnection(mainConn, forkConn)) {
        return NULL;
    }
    if (openConnFD(forkConn)) {
        return NULL;
    }
    conn_send(forkConn, MESSAGE_OK, strlen(MESSAGE_OK)+1);
    return forkConn;
}

static void listen_loop(Connection connection) {

    int isParentServer = 1;
    int done = 0;
    fd_set rfds;        //File Descriptor set for select() - has to be reconstructed every time
    struct timeval tv;  //Timeout struct for select()

    printf("This is main server #%i, listening to connections at %s\n", getpid(), connection->mainServerPath);
    printf("Listening for clients...");
    fflush(stdout);

    while(isParentServer || !done) {   
        setup(connection, &rfds, &tv);  
        int selectResult = select(connection->inFD + 1, &rfds, NULL, NULL, &tv);
        if(selectResult == 0) { /*Timed out*/
            printf(".");
            fflush(stdout);
        } else if (selectResult == -1) {  /*Error*/
            fprintf(stderr, "Error with select(). Aborting.\n");
        } else {
            
            Connection forkConn = createForkedConnection(connection);
            if (forkConn != NULL) {
                printf("client connected.\n");            
                if (fork() == 0) {   /*Child*/
                    isParentServer = 0;
                    conn_close(connection); /*TODO averiguar si esta bien */
                    forkedServer(forkConn);
                } else {  /*Parent*/
                    free(forkConn);
                    printf("Listening for clients...");
                    fflush(stdout);
                }
            }
        }
    }
    
}


static Connection client_conn_open(Connection connection) {

    char* ack;
    size_t len;

    if (createFIFOs(connection)) {
        return NULL;
    }

    /*if (openConnFD(connection)) {
        return NULL;
    } Can't open out path, why not?*/
    if ( (connection->inFD = openInFD(connection->inFD, connection->inFIFOPath)) < 0) {
        return NULL;
    }
    if (sendFIFOPaths(connection)) {
        return NULL;
    }

    len = strlen(MESSAGE_OK)+1;
    conn_receive(connection, (void **)&ack, &len);
    
    if(strcmp(ack, MESSAGE_OK)) {  //Server forked and listening, open write FIFO again
        fprintf(stderr, "Communication is corrupted\n");
        return NULL;
    }
    if ( (connection->outFD = openOutFD(connection->outFD, connection->outFIFOPath)) < 0) {
        return NULL;
    }
    return connection;
}


static Connection forked_server_conn_open(Connection connection) {

    return connection;
}

static Connection main_server_conn_open(Connection connection) {

    if (createMainServerFIFO(connection)) {
        return NULL;
    }
    if (openMainServerFD(connection)) {
        return NULL;
    }
    listen_loop(connection);
    return connection;
}

static Connection no_conn(Connection connection) {

    return NULL;
}

typedef Connection (*openConnectionFn)(Connection);


/*
 * Creates a connection between the current process and a server.
 * The procedure is as follows:
 *
 * 1) Client creates FIFO for reading (pid-in) and writing (pid-out), [DONE]
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
    
    Connection connection;
    int connectionType = checkConnection(address);
    openConnectionFn openConnection[4] = {&no_conn, &main_server_conn_open, &forked_server_conn_open, &client_conn_open};
    if (connectionType) {
        connection = malloc(sizeof(*connection));
        connection->mainServerPath = (char*)((void*)address + 1);
    }
    return openConnection[connectionType](connection); 
}

int conn_close(Connection connection) {
	conn_send(connection, MESSAGE_CLOSE, strlen(MESSAGE_CLOSE)+1);
	
    if (connection->outFD >= 0){
        if (close(connection->outFD) < 0) {
            fprintf(stderr, "Couldn't close connection\n");
            return -1;
        }
    }
    if (close(connection->inFD) < 0) {
        fprintf(stderr, "Couldn't close connection\n");
        return -1;
    }
    if (connection->outFIFOPath != NULL) {
        if (remove(connection->outFIFOPath) < 0) {
            fprintf(stderr, "Couldn't close connection\n");
            return -1;
        }
    }
    if (remove(connection->inFIFOPath) < 0) {
        fprintf(stderr, "Couldn't close connection\n");
        return -1;
    }

    printf("Connection closed.\n");
    free(connection->mainServerPath);
    free(connection->outFIFOPath);
    free(connection->inFIFOPath);
    free(connection);    
    return 0;
}

int conn_send(const Connection conn, const void* data, const size_t length) {
	if(length == 0) {
		return 1;
	}
	return ensureWrite(&length, sizeof(length), conn->outFD) && ensureWrite(data, length, conn->outFD);
}

int conn_receive(const Connection connection, void** data, size_t* length) {
	if(!ensureRead(length, sizeof(*length), connection->inFD)) {
		return 0;
	}
	*data = malloc(*length);
	if(!ensureRead(*data, *length, connection->inFD)) {
		if(*data != NULL) {
			free(*data);
		}
		return 0;
	}
    return 1;
}
