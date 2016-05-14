#include "dbConn.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int fds[2];
    if (pipe(fds) == -1) { //0 = read, 1 = write
        perror("pipe");
        return -1;
    }
    SharedDBConnection c = sh_conn_create(fds[1], fds[0]);
    int num = 0;
    write(sh_conn_get_out_fd(c), &num, sizeof (num));
    for (int i = 0; i < 100; i++) {
        if (fork() == 0) { //child
            sh_conn_open(c);
            read(sh_conn_get_in_fd(c), &num, sizeof (num));
            num++;
            printf("#%i = %i\n", getpid(), num);
            write(sh_conn_get_out_fd(c), &num, sizeof (num));
            sh_conn_close(c);
            exit(0);
        }
    }
    //Parent - wait until all children have written
    printf("Parent going zzz...\n");
    sleep(3);
    printf("Parent woke up\n");
    sh_conn_open(c);
    read(sh_conn_get_in_fd(c), &num, sizeof (num)); //Read it - NO TOUCHY
    printf("After sleeping, num = %i\n", num);
    write(sh_conn_get_out_fd(c), &num, sizeof (num)); //Now put it back
    if (sh_conn_destroy(c) == 0) {
        printf("Couldn't destroy connection.\n");
        return -1;
    }
    return 0;
}