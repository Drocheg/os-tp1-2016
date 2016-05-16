/*
 * forkedServer.h
 *
 *  Created on: Apr 23, 2016
 *      Author: jlp
 */

#ifndef FORKEDSERVER_H_
#define FORKEDSERVER_H_

#include <comm.h>
#include <dbConn.h>

extern SharedDBConnection dbConn;

struct connections_t {

	Connection clientConnection;
	SharedDBConnection *dbConn;

};

/**
 * Main loop for a forked server. The server will communicate with the
 * client in the specified connection.
 *
 * @param Connection c An established, working connection from which
 * to listen to requests.
 */
void forkedServer(Connection c);

#endif /* FORKEDSERVER_H_ */
