#include <comm.h>
#include <config.h>
#include <dbConn.h>
#include <logging.h>
#include <lib.h>
#include "forkedServer.h"

#include <sys/types.h>
//#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
//#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

SharedDBConnection dbConn = NULL;
int dbPID, logPID;

static int createForkedServer(Connection c) {
    int pid = fork();
    if (pid < 0) {
        log_warn("Couldn't create a forked server.");
        return -1;
    } else if (pid > 0) {
        /* Parent Process */
        return 0;
    } else {
        /* Child Process */
        forkedServer(c);
        return 0;
    }
}

/**
 * Starts the database server with a shared connection.
 * 
 * @param outFD The fd for writing to the database.
 * @param inFD The fd for reading from the database.
 * @return 1 on success, -1 on error.
 */
int startDBServer(int* outFD, int* inFD) {
    //Create pipes
    int outpipe[2], inpipe[2]; //pipe[0] =read, pipe[1] = write
    if (pipe(outpipe) == -1 || pipe(inpipe) == -1) {
        log_err("Main server couln't create pipes for shared connection,");
        return -1;
    }
    *outFD = outpipe[1];
    *inFD = inpipe[0];
    dbConn = sh_conn_create(*outFD, *inFD);
    if (dbConn == NULL) {
        log_err("Main server couldn't create shared connection.");
        return -1;
    }
    printf("OUT read: %i, write: %i\nIN read: %i, write: %i\n", outpipe[0], outpipe[1], inpipe[0], inpipe[1]);
    //Shared connection set up, fork and start database
    if ((dbPID = fork()) == 0) { //Child, start database server
        char dbReadFD[countDigits(outpipe[0]) + 1],
            dbWriteFD[countDigits(inpipe[1]) + 1];
        sprintf(dbReadFD, "%i", outpipe[0]);
        sprintf(dbWriteFD, "%i", inpipe[1]);
        char* args[4] = {"databaseServer.bin", dbReadFD, dbWriteFD, NULL};
        char* unused[] = {NULL};
        execve("/home/juan/tp1so/database/databaseServer.bin", args, unused);
        //If we're here, execve() failed.
        log_err("execve() failed when starting database server.");
        //TODO kill parent
        exit(-1);
    }
    return 1;

}

/**
 * Sends a message for the database server to shut down.
 * 
 * @param c The shared database connection. Will respect mutex.
 * @return 1 on success, -1 on error.
 */
int stopDBServer() {
    int msg = MESSAGE_CLOSE;
    sh_conn_open(dbConn);
    int writeResult = ensureWrite(&msg, sizeof(msg), sh_conn_get_out_fd(dbConn));
    if(writeResult) {
        dbConn = NULL;
    }
    sleep(1);   //TODO do waitpid or something
    sh_conn_destroy(dbConn);
    return writeResult ? 1 : -1;
}

/**
 * Starts the logging server.
 * 
 * @return 1 on success, -1 on error.
 */
int startLoggingServer() {
    if ((logPID = fork()) == 0) { //Child, start logging server
        char* unused[1] = {NULL};
        execve("/home/juan/tp1so/logging/loggingDaemon.bin", unused, unused);
        //If we're here, execve() failed.
        log_err("execve() failed when starting logging server.");
        //TODO kill parent
        exit(-1);
    }
    return 1;
}

/**
 * Shutdown procedure for server before actually ending the process.
 * - TODO send message to forked servers to shut down?
 * - Sends message to database server to shut down.
 */
static void shut_down() {
    if(stopDBServer() == -1) {
        log_warn("Couldn't send shutdown command to database server. Main server shutting down anyway.");
    }
    log_info("Bye-bye from main server.");
    log_info("Terminating log daemon.");
    sleep(2);   //TODO use waitpid to make sure all child processes are done before killing daemon, they will hang otherwise
    if(kill(logPID, 2) == -1) { //2 = SIGKILL. TODO where to find a #SIG_INT or something like that?
        printf("WARNING: Main server couldn't kill log daemon. Falling back to stdout.\n");
    }
}

int main(int argc, char *argv[]) {

    Config config = setup();


    int dbOutFD, dbInFD;
    //Start logging server
    if(startLoggingServer() == -1) {
        printf("Couldn't start logging server. Aborting.\n");
        exit(-1);
    }
    printf("Logging server started with PID #%i\n", logPID);
    //Start database server
    if (startDBServer(&dbOutFD, &dbInFD) == -1) {
        log_err("Main server aborting.");
        return -1;
    }
    printf("log PID: %i, db PID: %i\n", logPID, dbPID); //TODO remove, for debugging only
    char *address = getListeningPort(config);
    remove(address); //Remove it if it was already present (e.g. forcibly closed from a previous run)
    ConnectionParams connParams = conn_listen(address);
    if (connParams == NULL) {
        log_err("Main server couldn't start listening to connections. Aborting.");
        shut_down();
        return -1;
    }
    log_info("Main server started. Listening to connnection requests...");
    while (1) {
        Connection c = conn_accept(connParams);
        if (c == NULL) {
            log_err("Error accepting connection. Aborting.");
            shut_down();
            exit(1);
        } else {
            int pid = createForkedServer(c);
        }
    }
    shut_down();
    return 0;
}

