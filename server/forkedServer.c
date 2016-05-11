
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include "product.h"
#include "comm.h"
#include "config.h"
#include "order.h"

typedef int DbConnection; //TODO hacer esto bien

Product getProdcuts(DbConnection dbConnection);
int getNumProducts(DbConnection dbConnection);
int checkStockAndChange(Order order, DbConnection dbConnection);

void forkedServer(Connection c, DbConnection dbConnection);
void sendProducts(Connection c, DbConnection dbConnection);
void receiveOrder(Connection c, DbConnection dbConnection);
void endConnection(Connection c, DbConnection dbConnection);

void forkedServer(Connection c, DbConnection dbConnection){ //RequestProd 1 y  finish 2

	//De donde saco la base de datos?
	int done = 0;
	do{
		void * clienteData;
    	size_t msgLength;
    	conn_receive(c, &clienteData, &msgLength); //Al pedo msg lenght en este caso siempre int ?? O verificar que sea sizeOf int????
    	printf("msgLength : %d\n", msgLength);
        int msgCode = *((int*)clienteData);
        printf("msgCode : %d\n", msgCode);
    	switch(msgCode){//Rodrigo prefiere siempre cosas bien, paja
    		case 1:
    			sendProducts(c, dbConnection);
    			break;
    		case 2:	
    			receiveOrder(c, dbConnection);
    			break;
    		case 3:
    			endConnection(c, dbConnection);
                done = 1;
    			break;
    		default:
    			printf("WAT\n");
    			break;
    	}   
	}while(!done);
}


void sendProducts(Connection c, DbConnection dbConnection){

    
    Product product;
    product = getProdcuts(dbConnection); //TODO pedir productos a la base de datos
   
    Product * products;
    products = &product;

    int responseCode = 1;
    conn_send(c, &responseCode, sizeof(responseCode));


    int numProducts = getNumProducts(dbConnection);  //TODO pedir la cantidad de productos a la base de datos (Que sea correspondiente a los productos, sin cambios en el medio)
    conn_send(c, &numProducts,sizeof(numProducts));


    void * serializedProduct;
    size_t serializedProductSize;

    for(int i = 0; i<numProducts; i++){
        serializedProductSize=serializeProduct(products[i], &serializedProduct);
        printf("El tamaño del producto serializado: %d\n", serializedProductSize);
        conn_send(c,serializedProduct,serializedProductSize);
    } 
    
    printf("Ponele que te los mando\n");

}

void receiveOrder(Connection c, DbConnection dbConnection){

    
    void * serializedOrder;
    size_t serializedOrderSize;
    conn_receive(c, &serializedOrder, &serializedOrderSize);
    Order order = unserializeOrder(serializedOrder);

    int stockCheck = checkStockAndChange(order,dbConnection);//Se tiene que tener cuidado de que nadie aceda a la base de datos entre el check y el change
    if(stockCheck<0){ 
        printf("No hay stock\n");
        return; 
    }
    printf("Compra satisfactoria");

    int responseCode = 1;
    conn_send(c, &responseCode, sizeof(responseCode));
    
    printf("Ponele que funciona\n");
}


void endConnection(Connection c, DbConnection dbConnection){
    //TODO?
}
 
Product getProdcuts(DbConnection dbConnection){
    printf("Devuelve los productos\n");
    return newProduct("Vodka", "vodkaaa", 22, 2);
}

int getNumProducts(DbConnection dbConnection){
    return 1;
}

int checkStockAndChange(Order order, DbConnection dbConnection){
    return 1;
}
