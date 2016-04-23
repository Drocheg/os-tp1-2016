/* 
 * File:   main.c
 * Author: juan_
 *
 * Created on April 12, 2016, 9:54 PM
 */

#include <stdio.h>
#include <unistd.h> //For sleeping
#include <config.h>
#include <comm.h>

int main(int argc, char** argv) {
	printf("Connecting to server via %s...", getServerAddress());
	fflush(stdout);
    Connection c = conn_open(getServerAddress());
    printf("connected.\nSending random message...");
    fflush(stdout);
    conn_send(c, "Hello, server!", 15);
    printf("sent.\nSleeping...");
	fflush(stdout);
    sleep(5);
    printf("woke up.\nShutting down.\n");
	fflush(stdout);
    conn_close(c);
    return 0;
}
