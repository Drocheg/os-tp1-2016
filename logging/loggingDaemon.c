#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include "logging.h"

volatile int msqid; //TODO borrar volatile si no hace falta -- está para que el compilador no la cachee (http://stackoverflow.com/questions/246127/why-is-volatile-needed-in-c)
             
const char* LevelNames[] = {"INFO", "WARNING", "ERROR"};

void msgqueue_cleanup(int signo) {
    if (signo != SIGINT)
        return;

    printf("Log server received SIGINT, shutting down.\n");
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
    exit(0);
}

void prettyPrintMessage(message_t m) {
    printf("[%s]: %s\n", LevelNames[m.lvl-1], m.msg);
}

int main(void) {
    signal(SIGINT, msgqueue_cleanup);
    msqid = msgget(QUEUE_KEY, 0644 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Log server online! Waiting for messages...\n");
    // Receive messages forever.
    message_t currentMsg;
    while (1) {
        int status = msgrcv(msqid, &currentMsg, sizeof(currentMsg.msg), 0, 0);  //Will (happily) block waiting for messages
        if (status == -1) {
            perror("msgrcv");
            exit(1);
        }
        prettyPrintMessage(currentMsg);
    }
    return 0;
}