#ifndef message_h
#define message_h

#include "data.h"

#ifndef GET_PRODUCTS
#define GET_PRODUCTS 1
#endif

#ifndef CREATE_ORDER
#define CREATE_ORDER 2
#endif


typedef struct message_t * Message;
typedef struct arg_t * Argument;


/********************************************/
/*		Argument Manipulation Functions		*/
/********************************************/

/*
 * Returns the data type of the argument
 * In case no argument is specified, -1 is returned
 */
DataType getDataType(Argument argument);

/*
 * Returns the size of the data in the argument
 * In case no argument is specified, -1 is returned
 */
size_t getSize(Argument argument);

/*
 * Returns the data in the argument
 * In case no argument is specified, NULL is returned
 */
void* getArg(Argument argument);



/********************************************/
/*		Message Manipulation Functions		*/
/********************************************/

/*
 * Creates a new message with no arguments
 */
Message createMessage(int messageCode);

/*
 * Returns the method of the message.
 * In case no message is specified, -1 is returned
 */
int getMessageCode(Message message);

/*
 * Returns the number of arguments in the message.
 * In case no message is specified, -1 is returned
 */
int getArgc(Message message);

/*
 * Returns the arguments in the message.
 * In case no message is specified, NULL is returned
 */
Argument* getArguments(Message message);

/*
 * Adds an argument into the the message
 * Returns 0 on success, -1 otherwise (leaving the message as it was before calling)
 * If no message is specified, -1 is returned
 */
int addParam(Message message, DataType dataType, size_t size, void *arg); //Me copie del merca pero creo que Datatype = Data. Size duplicado?

/*
 * Deletes an argument from the message
 * Returns 0 on success, -1 otherwise (leaving the message as it was before calling)
 * Deleting a non-existent arguement is considered a success
 * If no message is specified, or if index is negative, -1 is returned
 */
int deleteParam(Message message, int index);



void sendMessage(Connexion c, Message message); //TODO


Message listenMessage(Connexion c); //TODO


#endif /* message_h */


