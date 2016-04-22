/* 
 * File:   config.h
 * Author: juan_
 *
 * Used for parsing the global config file. Only reads from config file.
 */

#ifndef CONFIG_H
#define CONFIG_H

typedef struct config_t * Config;

Config loadConfig();

char * getServerAddress();

#endif /* CONFIG_H */

