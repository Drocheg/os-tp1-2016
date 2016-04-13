/* 
 * File:   main.c
 * Author: juan_
 *
 * Created on April 12, 2016, 9:54 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "comm.h"

int main(int argc, char** argv) {
    printf("Server config FIFO: %s", getServerAddress());
    Connection c = openConnection(NULL);
    fflush(stdout);
    sleep(5);
    closeConnection(c);
    fflush(stdout);
    sleep(5);
    return (EXIT_SUCCESS);
}

