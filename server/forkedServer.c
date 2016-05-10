#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <lib.h>
#include <stdio.h>

#include "product.h"

void forkedServer(Connection c, Connection dbConnection);
void sendProducts(Connection c, Connection dbConnection);
void receiveOrder(Connection c, Connection dbConnection);
void endConnection(Connection c, Connection dbConnection);

void forkedServer(Connection c, Connection dbConnection){ //RequestProd 1 y  finish 2

	//De donde saco la base de datos?
	int done = 0;
	do{

		void * clienteData;
    	size_t msgLength;
    	conn_receive(c,clienteData,&msgLength); //Al pedo msg lenght en este caso siempre int ?? O verificar que sea sizeOf int????

    	int msgCode = *((int*)clienteData);
    	switch(msgCode){//Rodrigo prefiere siempre cosas bien, paja
    		case 1:
    			sendProducts(c, dbConnection);
    			break;
    		case 2:	
    			receiveOrder(c, dbConnection);
    			break;
    		case 3:
    			endConnection(c, dbConnection);
    			break;
    		default:
    			printf("WAT\n");
    			break;
    	}   
	}while(!done);
}


void sendProducts(Connection c, Connection dbConnection){

    Product * products;
    products = getProdcuts(dbConnection) //TODO pedir productos a la base de datos

    int responseCode = 1;
    conn_send(c, &responseCode, sizeof(responseCode));


    int numProducts = getNumProducts(dbConnection)  //TODO pedir la cantidad de productos a la base de datos (Que sea correspondiente a los productos, sin cambios en el medio)
    conn_send(c, &numProducts,sizeof(numProducts));


    void * serializedProduct;
    int serializedProductSize;

    for(int i = 0; i<*numProducts; i++){
        serializedProductSize=serializeProduct(products[i], serializedProduct);
        conn_send(c,serializedProduct,serializedProductSize);
    } 

}

void receiveOrder(Connection c, Connection dbConnection){

    void * serializedOrder;
    int serializedOrderSize;
    conn_receive(c, serializedOrder, &serializedOrderSize);
    Order order = unserializeOrder(serializedOrder);

    int stockCheck = checkStockAndChange(order,dbConnection);//Se tiene que tener cuidado de que nadie aceda a la base de datos entre el check y el change
    if(stockCheck<0){ 
        printf("No hay stock\n");
        return; 
    }
    printf("Compra satisfactoria");

    int responseCode = 1;
    conn_send(c, &responseCode, sizeof(responseCode));

}


void endConnection(Connection c, Connection dbConnection){
    //TODO
}