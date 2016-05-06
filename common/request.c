#include "request.h"
#include "data.h"


struct arg_t {
	
	DataType dataType;
	size_t size;
	void *arg;
};

struct request_t {

	int method;
	int argc
	struct arg_t *argv;
};



/*
 * Creates a new request with no arguments
 */
Request createRequest(int method) {

	Request request = malloc(sizeof(*request));
	request->method = method;
	request->argc = 0;
	request->argv = NULL;
	return request;
}


/*
 * Adds an argument into the the request
 * Returns 0 on success, -1 otherwise (leaving the request as it was before calling)
 */
int addParam(Request request, DataType dataType, size_t size, void *arg) {

	struct arg_t *argAux = realloc(request->argv, ((request->argc) + 1)*sizeof(struct arg_t));
	if (argAux == NULL) {
		return -1;
	}
	request->argv = argAux;
	request->argv[(request->argc)++] = {dataType, size, arg};
	return 0;
}


/*
 * Deletes an argument from the request
 * Returns 0 on success, -1 otherwise (leaving the request as it was before calling)
 * Deleting a non-existent arguement is considered a success
 */
int deleteParam(Request request, int index) {

	if (request->argv != NULL && index < request->argc) {
		struct arg_t *argAux = realloc(request->argv, ((request->argc) - 1)*sizeof(struct arg_t));
		if (argAux == NULL && (((request->argc) - 1) > 0) ) {
			return -1; /* In case a null pointer is returned and the request still has one or more arguemtns*/
		}
		while ((index + 1) < request->argc) {
			argAux[index] = request->argv[index + 1];
			index++;
		}
		(request->argc)--;
	}
	if (request->argc == 0) {
		free(request->argv);
		request->argv = NULL;
	}
	return 0;
}




















