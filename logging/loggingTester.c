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

int main(void)
{

  key_t key = QUEUE_KEY;
  int msqid = msgget(key, 0644);
  if (msqid == -1) {
    perror("msgget");
    printf("Couldn't connect to message queue.");
    exit(1);
  }

  printf("Successfully connected to the message queue! Sending test messages...\n\n");      
  int result;
  result = log_info("Test info message");
  if(result == -1) {
      printf("Error sending INFO message.\n");
  }
  result = log_warn("System temperatures rising");
  if(result == -1) {
      printf("Error sending WARNING message.\n");
  }
  result = log_err("It's the end of the world. Run for your lives!");
  if(result == -1) {
      printf("Error sending ERROR message.\n");
  }
  printf("Done. Bye-bye!\n");
  return 0;
}
