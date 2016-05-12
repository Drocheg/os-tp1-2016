#define _GNU_SOURCE

#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "logging.h"

int msqid = -1;

void connectToMsgQueue() {
    msqid = msgget(QUEUE_KEY, 0644);
}

int logMessage(const char* message, LogLevel lvl) {
    if(msqid == -1) {
        connectToMsgQueue();    //Try to connect (again if calling multiple times)
        if(msqid == -1) {
            return -1;
        }
    }
    message_t msg;
    msg.lvl = (long) lvl;
    strncpy(msg.msg, message, sizeof(msg.msg) - 1); //Don't overflow the message array
    return !msgsnd(msqid, &msg, sizeof(msg.msg), IPC_NOWAIT) ? 1 : -1;   //Don't block if the message couldn't be sent. 0 = success, -1 = error
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