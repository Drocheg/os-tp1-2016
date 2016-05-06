#ifndef request_h
#define request_h

#ifndef GET_PRODUCTS
#define GET_PRODUCTS 1
#endif

#ifndef CREATE_ORDER
#define CREATE_ORDER 2
#endif


typedef struct request_t * Request;


/*
 * Creates a new request with no arguments
 */
Request createRequest(int method);

/*
 * Adds an argument into the the request
 * Returns 0 on success, -1 otherwise (leaving the request as it was before calling)
 */
int addParam(Request request, DataType dataType, size_t size, void *arg);

/*
 * Deletes an argument from the request
 * Returns 0 on success, -1 otherwise (leaving the request as it was before calling)
 * Deleting a non-existent arguement is considered a success
 */
int deleteParam(Request request, int index);




#endif /* request_h