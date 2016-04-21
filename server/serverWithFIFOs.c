//tengo que leer de la direccion hardcodeada. 
//Cuando me llega algo va a ser la direccion in y out de nuevos pipes.
//Me forkeo y uso esos pipes para comunicarme.
//Puse todos los includes que habia en comm.c
#include "server.h"
#include "comm.h"
#include "config.h"
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

void listenToClients() {
    FILE *f = fopen(getServerAddress(), "r");
    //Crea el set para el select
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fileno(f), &rfds);
    printf("Server reading from %s (FD %i)\n", getServerAddress(), fileno(f));
    fflush(stdout);
    //Despues vemos si ponemos un timeOut para el servidor y cuanto. 
    //struct timeval tv;
    //tv.tv_sec = 5;
    //tv.tv_usec = 0;
    
    int retval = 0;
    int mainServer = 1;
    Connection connection = malloc(sizeof (*connection));
    
    while (mainServer) {
        printf("Waiting for select()\n");
        fflush(stdout);
        retval = select(fileno(f)+1, &rfds, NULL, NULL, NULL); //Ultimo parametro tendria que ser &tv para timeOut
        printf("select() Returned\n");
        fflush(stdout);
        if (retval == -1) {
            fprintf(stderr, "select()");
        } else {
            if (fork()) {
                printf("New Server\n");
                //Nuevo Servidor
                 mainServer = 0;
                
                //leer el len y despues leer el path. Primero in y despues out porque son al contrario que en el cliente.
                
                int lenIn = -1;
                int read = 0;
                while(read < 1) {
                    read += fread(&lenIn, sizeof(lenIn), 1, f);
                }
                connection->inFIFOPath = malloc(lenIn);
                //No se si tengo que pasar en realidad un puntero a connection->outFIFOpat  
                fread(connection->inFIFOPath, lenIn, 1, f);
                printf("Read %i bytes for IN fifo\n", lenIn);
                
                int lenOut = -1;
                read = 0;
                while(read < 1) {
                    read += fread(&lenOut, sizeof(lenOut), 1, f);
                }
                connection->outFIFOPath = malloc(lenOut);
                fread(connection->outFIFOPath, lenOut, 1, f);
                printf("Read %i bytes for OUT fifo\n", lenOut);
                
                printf("Writing to %s\n", connection->outFIFOPath);
                printf("Reading from %s\n", connection->inFIFOPath);
                //Se tiene la nueva conexion en este nuevo servidor
                //Tomar los 2 FIFOS y hacer algo.
                
            }
            else {
                //Servidor principal
                //Ignorar los 2 FIFOS y volver
                //Hacer algo?
            }
        }
        
        printf("New Client Connection");
        char *msj = "Hello?";
        conn_send(connection, msj, strlen(msj)+1);
    }
}