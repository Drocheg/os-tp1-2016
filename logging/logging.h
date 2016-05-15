#ifndef logger_h
#define logger_h

typedef struct {        //TODO should this be here? Both server and logging.c need it
    long lvl;
    char msg[256]; //Fixed max message length
} message_t;

typedef enum {INFO = 1, WARNING, ERROR} LogLevel;

// Some seemingly random long for this message queue. Easier to use this than
// have a common readable file.
#define QUEUE_KEY 0x95134529

/**
 * Logs an INFO-level message.
 * 
 * @param msg The message to log.
 * @return 1 on success, -1 on failure.
 */
int log_info(const char* msg);

/**
 * Logs a WARNING-level message.
 * 
 * @param msg The message to log.
 * @return 1 on success, -1 on failure.
 */
int log_warn(const char* msg);

/**
 * Logs an ERROR-level message.
 * 
 * @param msg The message to log.
 * @return 1 on success, -1 on failure.
 */
int log_err(const char* msg);

#endif
