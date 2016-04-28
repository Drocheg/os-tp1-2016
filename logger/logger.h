#ifndef logger_h
#define logger_h

typedef struct {
  long mtype;
  char msg[255];
} log_msgbuf;

#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3


// Some seemingly random long for this message queue. Easier to use this than
// have a common readable file.
#define QUEUE_KEY 0x95134529

#endif
