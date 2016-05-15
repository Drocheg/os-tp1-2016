#include "forkedServer.h"
#include <config.h>
#include <product.h>
#include <order.h>
#include <lib.h>
#include <logging.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

int getProdcuts(Product **destArray);

void sendProducts(Connection c);

void processOrder(Connection c);

static void shut_down(Connection c);

void forkedServer(Connection c) { //RequestProd 1 y  finish 2
    //De donde saco la base de datos?
    int done = 0;
    do {
        void* clientData;
        conn_receive(c, &clientData, NULL);
        int msgCode = *((int*) clientData);
        free(clientData);
        switch (msgCode) {      //TODO use function array?
            case CMD_GET_PRODUCTS:
                sendProducts(c);
                break;
            case CMD_PLACE_ORDER:
                processOrder(c);
                break;
            case MESSAGE_CLOSE:
                shut_down(c);
                done = 1;
                break;
            default:
                log_warn("Forked server received unknown message code. Behavior might be wacky from now on.");
                break;
        }
    } while (!done);
    exit(0);
}

void sendProducts(Connection c) {
    Product* products;
    int numProducts = getProdcuts(&products);
    int responseCode;
    if(numProducts == -1) {
        responseCode = MESSAGE_ERROR;
        conn_send(c, &responseCode, sizeof (responseCode));
    }
    //Send product count
    conn_send(c, &numProducts, sizeof (numProducts));
    //Send each product
    for (int i = 0; i < numProducts; i++) {
        void *serialized;
        size_t serializedLen;
        serializedLen = serializeProduct(products[i], &serialized);
        conn_send(c, serialized, serializedLen);
        free(serialized);
    }
}

void processOrder(Connection c) {
    //Read order
    void* serialized;
    size_t serializedLen;
    conn_receive(c, &serialized, &serializedLen);
    //No need to serialize order
    
    //Place order and parse response
    sh_conn_open(dbConn);
    int code = CMD_PLACE_ORDER,
        outFD = sh_conn_get_out_fd(dbConn),
        inFD = sh_conn_get_in_fd(dbConn);
    ensureWrite(&code, sizeof(code), outFD);
    ensureWrite(&serializedLen, sizeof(serializedLen), outFD);
    ensureWrite(serialized, serializedLen, outFD);
    free(serialized);
    //Wait for response...
    ensureRead(&code, sizeof(code), inFD);
    //Send back response code
    conn_send(c, &code, sizeof(code));
    //If needed, send back extra data
    if(code == MESSAGE_UNSATISFIABLE_ORDER) {
        //Read new serialized order
            ensureRead(&serializedLen, sizeof(serializedLen), inFD);
            serialized = malloc(serializedLen);
            ensureRead(serialized, serializedLen, inFD);
            //Send it back
            conn_send(c, &code, sizeof(code));
            conn_send(c, serialized, serializedLen);
    }
    sh_conn_close(dbConn);
}

void shut_down(Connection c) {
    log_info("Forked server shutting down.");
    conn_close(c);
}

int getProdcuts(Product **destArray) {
    int outFD = sh_conn_get_out_fd(dbConn),
        inFD = sh_conn_get_in_fd(dbConn);
    sh_conn_open(dbConn);
    //Request products
    int code = CMD_GET_PRODUCTS;
    if(!ensureWrite(&code, sizeof(code), outFD)) {
        return -1;
    }
    int readResult;
    if(!(readResult = ensureRead(&code, sizeof(code), inFD)) || code == MESSAGE_ERROR) {
        return -1;
    }
    //Code contains the number of products returned.
    *destArray = malloc(sizeof(**destArray)*code);
    for(int i = 0; i < code; i++) {
        size_t serializedLen;
        ensureRead(&serializedLen, sizeof(serializedLen), inFD);    //TODO handle failure
        void* serialized = malloc(serializedLen);
        ensureRead(serialized, serializedLen, inFD);    //TODO handle failure
        (*destArray)[i] = unserializeProduct(serialized);
        free(serialized);
    }
    sh_conn_close(dbConn);
    return code;
}