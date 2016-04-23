#include <stdio.h>
#include "comm.h"


int main(int argc, char *argv[]) {

	Connection connection = conn_open(argv[1]);
	printf("Good bye!\n");
	return 0;
}