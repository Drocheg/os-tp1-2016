/* 
 * Dummy client. Connects, waits a random amount of time and disconnects.
 * Used for stress-testing server. Compile with FIFOs or sockets to test each
 * method.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleeping
#include <config.h>
#include <comm.h>

int main(int argc, char** argv) {
    //TODO doesn't work, make it werk!
    char *addr = getServerAddress();
    int pid = getpid();
    printf("Dummy client #%i connecting...", pid);
    fflush(stdout);
    Connection c = conn_open(addr);
    if(c == NULL) {
        printf("dummy client #%i couldn't connect. Aborting.\n", pid);
        return -1;
    }
    printf("connected.\n");
    int min = 1, max = 10, waitSec = (int)(rand() / ( (double)RAND_MAX + 1 )*(max-min+1)+min);
    printf("Dummy client #%i sleeping for %i sec...", pid, waitSec);
    fflush(stdout);
    sleep(waitSec);
    printf("Dummy client #%i woke up, disconnecting...", pid);
    fflush(stdout);
    conn_close(c);
    printf("done. Bye bye from dummy client #%i\n", pid);
    return 0;
}