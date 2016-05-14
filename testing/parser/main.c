#include "configFile.h"
#include <stdio.h>


int main(void) {
	
	Config config = setup();
	printf("Server: %s\n", getServerAddress(config));
	printf("Port: %s\n", getListeningPort(config));
	printf("Database: %s\n", getDatabaseAddress(config));
	printf("Logging: %s\n", getLoggingAddress(config));

	return 0;

}