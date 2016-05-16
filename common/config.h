#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H


#ifndef CONF_FILE_PATH
#define CONF_FILE_PATH "/home/parallels/TPSO/config.conf"
#endif

typedef struct config_t * Config;


Config setup();

char *getServerAddress(Config config);

char *getListeningPort(Config config);

char *getDatabaseAddress(Config config);

char *getLoggingAddress(Config config);




















#endif /*CONFIG_FILE_H*/
