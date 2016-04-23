#include <stdio.h>
#include <stdlib.h>


char * getLine() {

	char * aux = NULL;
	char * result = NULL;
	int i = 0;
	do {
		if (i % 10 == 0) {
			aux = realloc(result, 10);
			result = aux;
		}
		result[i] = getchar();

	} while (aux[i++] != '\n')
		;
	result[i - 1] = 0;
	return result;
}