#include "data.h"
#include <stdlib.h>
#include <string.h>


struct data_t {

	size_t size;
	int dataType;
	void * data;
};


Data newData(void * data, size_t size, int dataType) {
	
	Data result = malloc(sizeof(*data));
	
	result->data = data;
	result->size = size;
	result->dataType = dataType;

	return result;
}

void * getData(Data data) {
	
	return data->data;
}

int getSize(Data data) {

	return data->size;
}

int getDataType(Data data) {

	return data->dataType;
}

/*
 * Transforms a data structure in an array of bytes
 * the first ones are the size of the encapsulated data
 * Afterwards comes the data type
 * Finally, the encapsulated data
 */
void* serialize(Data data, serializerFn serializer) {
	void* aux = malloc(data->size + sizeof(int) + sizeof(size_t));
	int i = 0;
	memcpy(aux, &(data->size), sizeof(size_t));
	i += sizeof(size_t);
	memcpy(aux + i, &(data->dataType), sizeof(int));
	i += sizeof(int);
	memcpy(aux + i , serializer(data->data), data->size);

	return aux;

}

/*
 * Transforms an array of bytes into a data structure
 */
Data unserialize(void* bytes, serializerFn serializer) {

	void * data;
	size_t size = *(size_t*)bytes; /* First bytes are the encapsulated data size*/
	bytes += sizeof(size_t);
	int dataType = *(int*)(bytes);
	bytes += sizeof(int);
	memcpy(data, bytes, size);
	return newData(data, size, dataType);

}


