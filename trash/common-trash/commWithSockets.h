/*
 * commWithSockets.h
 *
 *  Created on: Apr 23, 2016
 *      Author: jlp
 */

#ifndef COMMON_COMMWITHSOCKETS_H_
#define COMMON_COMMWITHSOCKETS_H_

#include "comm.h"

struct connection_t {
	char* ip;
	in_port_t port;
	int socketfd;

};

#endif /* COMMON_COMMWITHSOCKETS_H_ */
