#ifndef start_h
#define start_h

#include <sys/types.h>

#define RAWDATA 0
#define STRING 1

typedef struct data_t * Data;


Data newData(void * data, size_t size, int dataType);

void * getData(Data data);

int getSize(Data data);

int getDataType(Data data);


#endif /* start_h */