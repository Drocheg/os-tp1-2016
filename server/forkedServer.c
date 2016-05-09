#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <lib.h>
#include <stdio.h>

#include "product.h"




void forkedServer(Connection c){ //RequestProd 1 y  finish 2

	//De donde saco la base de datos?
	int done = 0;
	do{

		void * clienteData;
    	size_t msgLength;
    	conn_receive(c,clienteData,&msgLength); //Al pedo msg lenght en este caso siempre int ?? O verificar que sea sizeOf int????

    	int msgCode = *((int*)clienteData);
    	switch(msgCode){//Rodrigo prefiere siempre cosas bien, paja
    		case 1:
    			sendProducts(c);
    			break;
    		case 2:	
    			receiveOrder(c);
    			break;
    		case 3:
    			endConnection(c);
    			break;
    		default:
    			printf("WAT\n");
    			break;
    	}   
	}while(!done);
}