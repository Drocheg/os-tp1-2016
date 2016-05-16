#include <comm.h>
#include <config.h>
#include <logging.h>
#include <lib.h>
#include "forkedServer.h"

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

SharedDBConnection dbConn = NULL;
pid_t dbPID, logPID, mainServerPID;

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
    
    //Shared connection set up, fork and start database
    if ((dbPID = fork()) == 0) { //Child, start database server
        char dbReadFD[countDigits(outpipe[0]) + 1],
            dbWriteFD[countDigits(inpipe[1]) + 1];
        sprintf(dbReadFD, "%i", outpipe[0]);
        sprintf(dbWriteFD, "%i", inpipe[1]);
        char* args[4] = {"databaseServer.bin", dbReadFD, dbWriteFD, NULL};
        char* unused[] = {NULL};
        execve("./databaseServer.bin", args, unused);
        //If we're here, execve() failed.
        log_err("execve() failed when starting database server.");
        kill(mainServerPID, SIGKILL);
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
 * Shutdown procedure for server before actually ending the process.
 * - TODO send message to forked servers to shut down?
 * - Sends message to database server to shut down.
 */
static void shut_down() {
    if(stopDBServer() == -1) {
        log_warn("Couldn't send shutdown command to database server. Main server shutting down anyway.");
    }
    log_info("Bye-bye from main server.");
     
}

int main(int argc, char *argv[]) {

	system("clear");
    Config config = setup();
    mainServerPID = getpid();


    int dbOutFD, dbInFD;
    //Start database server
    if (startDBServer(&dbOutFD, &dbInFD) == -1) {
        log_err("Main server aborting.");
        return -1;
    }
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

