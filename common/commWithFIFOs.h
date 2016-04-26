/*
 * commWithFIFOs.h
 *
 *  Created on: Apr 23, 2016
 *      Author: jlp
 */

#ifndef COMMON_COMMWITHFIFOS_H_
#define COMMON_COMMWITHFIFOS_H_

#include "comm.h"

//TODO Delete this file, ideally this should only be in commWithFIFOs.c and nowhere else

struct connection_t {
    char* outFIFOPath;
    char* inFIFOPath;
    int outFD;
    int inFD;
};

struct conn_params_t {
    int connRequestsFD;   // File descriptor of open file where to listen for connection requests
};

#endif /* COMMON_COMMWITHFIFOS_H_ */
