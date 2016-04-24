#include "config.h"
#include "lib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PATH "configWithFIFOs.txt"

struct config_t {
    char * serverFIFOPath;
};

/**
 * Reads data from the config file and returns the info in a structure.
 * 
 * @return config A structure containing the recognized config options from the
 * global config file.
 */
Config loadConfig() {
    Config result = malloc(sizeof(struct config_t));
    FILE *configFile = fopen(PATH, "r");
    int buffSize = 1024;
    char buff[buffSize];
    if(configFile == NULL) {
        return NULL;
    }
    if(fgets(buff, buffSize, configFile) != NULL) {
        //Not doing annoying parsing now, line 1 is the server FIFO y no jodan por ahora
        result->serverFIFOPath = malloc(sizeof(buff)+1);
        strcpy(result->serverFIFOPath, buff);
//        int separatorIndex = indexOf(buff, ":");
//        char key[separatorIndex+2], value[buffSize-separatorIndex+2];
//        key = memcpy(key, buff, separatorIndex)
    }
    fclose(configFile);
    return result;
}

char * getServerAddress() {
    return "/tmp/mainServerFIFO";//loadConfig()->serverFIFOPath;
}
