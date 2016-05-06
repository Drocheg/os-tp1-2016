#include "request.h"


struct arg_t {
	
	DataType dataType;
	size_t size;
	void *arg;
};

static Argument createArgument(DataType dataType, size_t size, void *arg) {
	
	Argument argument = malloc(sizeof(*argument));
	argument->dataType = dataType;
	argument->size = size;
	argument->arg = arg;
	return argument;
}

DataType getDataType(Argument argument) {
	return (argument == NULL) ? -1 : argument->dataType;
}

size_t getSize(Argument argument) {
	return (argument == NULL) ? -1 : argument->size;
}

void* getArg(Argument argument) {
	return (argument == NULL) ? NULL : argument->arg;
}



struct request_t {

	int method;
	int argc;
	Argument *argv;
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


int getMethod(Request request) {
	return (request == NULL) ? -1 : request->method;
}

int getArgc(Request request) {
	return (request == NULL) ? -1 : request->argc;
}

Argument* getArguments(Request request) {
	return (request == NULL) ? NULL : request->argv;
}



/*
 * Adds an argument into the the request
 * Returns 0 on success, -1 otherwise (leaving the request as it was before calling)
 */
int addParam(Request request, DataType dataType, size_t size, void *arg) {

	if (request == NULL) {
		return -1;
	}

	Argument *argAux = realloc(request->argv, ((request->argc) + 1)*sizeof(Argument));
	if (argAux == NULL) {
		return -1;
	}
	request->argv = argAux;
	request->argv[(request->argc)++] = createArgument(dataType, size, arg);
	return 0;
}


/*
 * Deletes an argument from the request
 * Returns 0 on success, -1 otherwise (leaving the request as it was before calling)
 * Deleting a non-existent arguement is considered a success
 */
int deleteParam(Request request, int index) {

	if (request == NULL || index < 0) {
		return -1;
	}

	if (request->argv != NULL && index < request->argc) {
		Argument argAux = realloc(request->argv, ((request->argc) - 1)*sizeof(Argument));
		if (argAux == NULL && (((request->argc) - 1) > 0) ) {
			return -1; /* In case a null pointer is returned and the request still has one or more arguemtns*/
		}
		free(request->argv[index]);
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


