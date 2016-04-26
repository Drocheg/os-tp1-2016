/* 
 * File:   main.c
 * Author: juan_
 *
 * Created on April 12, 2016, 9:54 PM
 */

#include <lib.h>
#include <stdio.h>
#include <unistd.h> // For sleeping
#include <config.h>
#include <comm.h>

#include <product.h>



int main(int argc, char** argv) {
    printf("Connecting to server via %s...", getServerAddress());
    fflush(stdout);
    Connection c = conn_open(getServerAddress());
    printf("connected.\n");
    printf("Welcome to IAC (Inter-Alcohol Communication)!\n");
    int done = 0, option;
    do {
        option = scanInt("What would you like to do?\n\t1) Browse products\n\t2) Purchase selected products\n\t3) Exit\n\n");
        switch(option) {
            case 1:
                //TODO show products
                prettyPrintProduct(newProduct("Vodka", "Vodkaaaaa", 42.50, 3));
                done = 1;
                break;
            case 2:
                //TODO check if there are items
//                done = 1;
                break;
            case 3:
                done = 1;
                break;
            default:
                printf("Invalid option selected.\n");
                break;
        }
    } while(!done);
    
//    fflush(stdout);
//    conn_send(c, "Hello, server!", 15);
//    printf("sent.\nSleeping...");
//	fflush(stdout);
//    sleep(5);
    printf("Disconnecting...");
    fflush(stdout);
    conn_close(c);
    printf("done.\n");
    printf("See you next time!\n");
    return 0;
}
