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

int main(void)
{

  key_t key = QUEUE_KEY;
  int msqid = msgget(key, 0644);
  if (msqid == -1) {
    perror("msgget");
    exit(1);
  }

  printf("Successfully got the message queue ID! Sending test messages...\n\n");

  // Recieve messages forever.
  log_msgbuf buf1;
  buf1.mtype = LOG_LEVEL_INFO;
  strncpy(buf1.msg, "Just a test", sizeof(buf1.msg));

  log_msgbuf buf2;
  buf2.mtype = LOG_LEVEL_WARN;
  strncpy(buf2.msg, "System temperatures rising", sizeof(buf2.msg));

  log_msgbuf buf3;
  buf3.mtype = LOG_LEVEL_ERROR;
  strncpy(buf3.msg, "It's the end of the world. Run for your lives!", sizeof(buf3.msg));

  msgsnd(msqid, &buf1, sizeof(buf1.msg), 0);
  msgsnd(msqid, &buf2, sizeof(buf2.msg), 0);
  msgsnd(msqid, &buf3, sizeof(buf3.msg), 0);

  return 0;
}
