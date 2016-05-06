#ifndef request_h
#define request_h

#include "data.h"

#ifndef GET_PRODUCTS
#define GET_PRODUCTS 1
#endif

#ifndef CREATE_ORDER
#define CREATE_ORDER 2
#endif


typedef struct request_t * Request;
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
/*		Request Manipulation Functions		*/
/********************************************/

/*
 * Creates a new request with no arguments
 */
Request createRequest(int method);

/*
 * Returns the method of the request.
 * In case no request is specified, -1 is returned
 */
int getMethod(Request request);

/*
 * Returns the number of arguments in the request.
 * In case no request is specified, -1 is returned
 */
int getArgc(Request request);

/*
 * Returns the arguments in the request.
 * In case no request is specified, NULL is returned
 */
Argument* getArguments(Request request);

/*
 * Adds an argument into the the request
 * Returns 0 on success, -1 otherwise (leaving the request as it was before calling)
 * If no request is specified, -1 is returned
 */
int addParam(Request request, DataType dataType, size_t size, void *arg);

/*
 * Deletes an argument from the request
 * Returns 0 on success, -1 otherwise (leaving the request as it was before calling)
 * Deleting a non-existent arguement is considered a success
 * * If no request is specified, or if index is negative, -1 is returned
 */
int deleteParam(Request request, int index);




#endif /* request_h