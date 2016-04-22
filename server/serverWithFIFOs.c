//tengo que leer de la direccion hardcodeada. 
//Cuando me llega algo va a ser la direccion in y out de nuevos pipes.
//Me forkeo y uso esos pipes para comunicarme.
//Puse todos los includes que habia en comm.c
#include <comm.h>
#include <config.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <lib.h>

int main(int argc, char **argv) {
    //Create main server FIFO
    char *FIFOpath = getServerAddress();
    remove(FIFOpath);
    if(mkfifo(FIFOpath, 0666)) {	//-1 = error
        if(remove(FIFOpath) || mkfifo(FIFOpath, 0666)) {
            fprintf(stderr, "Error creating main server FIFO.");
            return -1;
        }
    }
    int mainServerFD = open(FIFOpath, O_RDONLY|O_NONBLOCK);
    if(mainServerFD == -1) {
        fprintf(stderr, "Couldn't open main server FIFO. Aborting.");
        return -1;
    }
    //FIFO was opened in non-blocking mode (otherwise server gets stuck) but now
    //we want it to be blocking. Remove nonblock flag.
	int flagThing = fcntl(mainServerFD, F_SETFL, fcntl(mainServerFD, F_GETFL)&~O_NONBLOCK);	//Remove nonblocking flag
    //Crea el set para el select
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(mainServerFD, &rfds);
    printf("Server reading from %s (FD %i)\n", FIFOpath, mainServerFD);
    fflush(stdout);
    //Despues vemos si ponemos un timeOut para el servidor y cuanto. 
    //struct timeval tv;
    //tv.tv_sec = 5;
    //tv.tv_usec = 0;
    int isParentServer = 1;
    int done = 0;
    while(isParentServer || !done) {
        printf("Waiting for clients...");
        fflush(stdout);
        int retval = select(mainServerFD+1, &rfds, NULL, NULL, NULL); //Ultimo parametro tendria que ser &tv para timeOut
        printf("client connected.\n");
        if(retval == 0) {
        	printf("select() reports no open file descriptors.\n");
			return -1;
        }
        else if (retval == -1) {
            printf("Error with select()\n");
            return -1;
        }
        else {
            if (fork()) {   //Child
                isParentServer = 0;
                printf("This is forked server #%i\n", getpid());
                Connection connection = malloc(sizeof(*connection));
                //Read out pipe (in for server)
                int lenIn = -1;
                ensureRead(&lenIn, sizeof(lenIn), mainServerFD);
                connection->inFIFOPath = malloc(lenIn);
                ensureRead(connection->inFIFOPath, lenIn, mainServerFD);
                printf("Read %i bytes for in pipe: %s\n", lenIn, connection->inFIFOPath);
                //Read in pipe (out for server)
                int lenOut = -1;
                ensureRead(&lenOut, sizeof(lenOut), mainServerFD);
                connection->outFIFOPath = malloc(lenOut);
                ensureRead(connection->outFIFOPath, lenOut, mainServerFD);
                printf("Read %i bytes for out pipe: %s\n", lenOut, connection->outFIFOPath);
                //Don't need main server FIFO anymore
//                close(mainServerFD);
                //Complete connection set-up
                int inFD = open(connection->inFIFOPath, O_RDONLY|O_NONBLOCK);
                flagThing = fcntl(inFD, F_SETFL, fcntl(inFD, F_GETFL)&~O_NONBLOCK);	//Remove nonblocking flag
                connection->inFD = inFD;
                int outFD = open(connection->outFIFOPath, O_WRONLY);	//This should not fail, client has opened in read mode
                connection->outFD = outFD;
                conn_send(connection, MESSAGE_OK, strlen(MESSAGE_OK)+1);	//Send ACK
                while(!done) {
                    int msgLen = 0;
                    char *msg = NULL;
                    conn_receive(connection, &msg, &msgLen);
                    if(strcmp(msg, MESSAGE_CLOSE) == 0) {
                        printf("Received %s, shutting down forked server #%i\n", MESSAGE_CLOSE, getpid());
                        fflush(stdout);
                        //Don't use conn_close, the client called that function and we shouldn't use their pipes.
                        //Free up resources
                        close(inFD);
                        close(outFD);
                        free(connection->inFIFOPath);
                        free(connection->outFIFOPath);
                        free(connection);
                        done = 1;
                    }
                    else {
                        printf("Received %s\n", msg);
                    }
                    if(msg != NULL) {
                        free(msg);
                    }
                }
            }
            else {
                //Servidor principal
                //Ignorar los 2 FIFOS y volver
                //Hacer algo?
            }
        }
    }
    if(isParentServer) {
		printf("Shutting down main server #%i.\n", getpid());
		close(mainServerFD);
		remove(getServerAddress());
    }
    return 0;
}
