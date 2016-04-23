/*
 * commWithFIFOs.h
 *
 *  Created on: Apr 23, 2016
 *      Author: jlp
 */

#ifndef COMMON_COMMWITHFIFOS_H_
#define COMMON_COMMWITHFIFOS_H_

#include "comm.h"

struct connection_t {
    char* outFIFOPath;
    char* inFIFOPath;
    int outFD;
    int inFD;
};

#endif /* COMMON_COMMWITHFIFOS_H_ */
