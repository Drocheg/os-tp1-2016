#define _GNU_SOURCE

#include "logging.h"

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>


int msqid = -1;

void connectToMsgQueue() {
    msqid = msgget(QUEUE_KEY, 0644);
}

int logMessage(const char* message, LogLevel lvl) {
    if(msqid == -1) {
        connectToMsgQueue();    //Try to connect (again if calling multiple times)
        if(msqid == -1) {
            printf("Logging failed. Falling back to stdout:\n[%li]: %s\n", (long)lvl, message);
            return -1;
        }
    }
    message_t msg;
    msg.lvl = (long) lvl;
    strncpy(msg.msg, message, sizeof(msg.msg) - 1);                             //Don't overflow the message array
    int sendResult = msgsnd(msqid, &msg, sizeof(msg.msg), IPC_NOWAIT);          //Don't block if the message couldn't be sent. 0 = success, -1 = error
    if(sendResult == -1) {
        printf("Logging failed. Falling back to stdout:\n[%li]: %s\n", msg.lvl, msg.msg);
    }
    return sendResult == -1 ? -1 : 1;
}

int log_info(const char* msg) {
    return logMessage(msg, INFO);
}

int log_warn(const char* msg)  {
    return logMessage(msg, WARNING);
}

int log_err(const char* msg)   {
    return logMessage(msg, ERROR);
}