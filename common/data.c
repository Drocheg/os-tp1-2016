#include "data.h"
#include <stdlib.h>


struct data_t {

	void * data;
	size_t size;
	int dataType;
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


