#ifndef data_h
#define data_h

#include <sys/types.h>

#define RAWDATA 0
#define STRING 1
#define REQUEST 2

typedef struct data_t * Data;

/*
 * Function to serialize an object
 * Returns a pointer to an array of bytes representing an object
 */
typedef void* (*serializerFn)(void * object);

/*
 * Function to unserialize an object
 * Returns the object (i.e. a pointer to the object)
 */
typedef void* (*unserializerFn)(void * bytes);


Data newData(void* data, size_t size, int dataType);

void* getData(Data data);

int getSize(Data data);

int getDataType(Data data);

void* serialize(Data data, serializerFn serializer);

Data unserialize(void* bytes, serializerFn serializer);


#endif /* data_h */