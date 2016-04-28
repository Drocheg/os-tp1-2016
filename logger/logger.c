#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include "logger.h"

volatile int msqid;

void msgqueue_cleanup(int signo)
{
  if (signo != SIGINT)
    return;

  if (msgctl(msqid, IPC_RMID, NULL) == -1) {
    perror("msgctl");
    exit(1);
  }

  exit(0);
}

int main(void)
{
  printf("Logging daemon started!\n");
  signal(SIGINT, msgqueue_cleanup);

  key_t key = QUEUE_KEY;
  msqid = msgget(key, 0644 | IPC_CREAT);
  if (msqid == -1) {
    perror("msgget");
    exit(1);
  }

  printf("Successfully got the message queue ID! Waiting for messages...\n\n");

  // Recieve messages forever.
  log_msgbuf buf;
  while (1) {
    int status = msgrcv(msqid, &buf, sizeof(buf.msg), 0, 0);
    if (status == -1) {
      perror("msgrcv");
      exit(1);
    }

    switch (buf.mtype) {
    case LOG_LEVEL_INFO:
      printf("[INFO] %s\n", buf.msg);
      break;
    case LOG_LEVEL_WARN:
      printf("[WARN] %s\n", buf.msg);
      break;
    case LOG_LEVEL_ERROR:
      printf("[ERROR] %s\n", buf.msg);
      break;
    default:
      printf("[UNKNOWN] %s\n", buf.msg);
      break;
    }
  }

  return 0;
}
