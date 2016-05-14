/* 
 * File:   main.c
 * Author: juan_
 *
 * Created on April 12, 2016, 9:54 PM
 */

#include <lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <config.h>
#include <comm.h>
#include <product.h>
#include <order.h>

void startPurchase(int index, Product *products, Order order);
void finishPurchase(Order order, Connection c);
void printProducts(Product *products, int num);
void addProduct(Product product, Order order, int num);
int requestProducts(Product ** products, int * numProducts, Connection c);
void endConnection(Connection c);

 /**
Voy a hacer que le pida los productos al servidor, despues hace todo un sistema de compra 
donde se genera un "carro" de compras y al final se lo manda y el server lo checkea
El carro de compras va a ser una lista de ints en donde en cada nodo pongo el numero del producto y la cantidad.
Haria que sea un vector pero seria re poco escalable. Porque no es raro que un local tenga muchos productos distintos pero si es 
raro que un comprador compre muchos productos distintos. 
 **/

int main(int argc, char** argv) {
    printf("Connecting to server via %s...", getServerAddress());
    fflush(stdout);
    Connection c = conn_open(getServerAddress());
    if(c == NULL) {
        printf("Couldn't connect. Aborting.\n");
        return -1;
    }
    printf("connected.\n");
    printf("Welcome to IAC (Inter-Alcohol Communication)!\n");
    int done = 0, option;
    Order order = newOrder(); //Esto es el resumen de las ordenes. Maxima cantidad de diferentes productos es 16.  
    Product *products;//Esto tiene los productos que le manda la base de dato. 
    int numProducts = 0;
    requestProducts(&products, &numProducts, c); //TODO deberia pedirle los productos al servidor.
    do {
        printProducts(products, numProducts); 
        option = scanInt("Use the numbers to select the product you would like to purchase\n Press 0 to Exit and 1 to Finish your purchase\n"); //TODO despues vemos la tecla y eso bien. Como hacer con mucho productos bla bla.
                
        switch(option) {
            case 0:
                done = 1;
                endConnection(c);
                break;
            case 1:
                finishPurchase(order, c);
                //TODO que pasa si hay un error en la compra por desincronisacion? Avisar que hay algo mal y re imprimir.
                //No sale, sale con el 0. Esto es para confirmar compra. ???
                break;
            default:
                if(option > 0 && option-2 < numProducts) { //Because there are N products and 0 is exit. The products are the entries from 1 to N.
                    startPurchase(option-2, products, order); //Entrar en la etapa de comprar. Se pasa opcion-2 porque ese es el indice en el array.
                }
                else{
                    printf("Invalid option selected.\n");   
                }  
                break;
        }

    } while(!done);

    printf("Disconnecting...");
    fflush(stdout);
    conn_close(c);
    printf("done.\n");
    printf("See you next time!\n");
    freeOrder(order);
    return 0;
}

void startPurchase(int index, Product *products, Order order) {
    int done = 0;
    int num = 0;
    //Verda:  
    
    do{
        prettyPrintProduct(products[index]);
        num=scanInt("How many do you want to buy?\n Send 0 to cancel\n"); 
        if(num<0 || num>getProductStock(products[index])) {
            printf("Need to put a valid quantity");
        }
        else {
            done=1;
            if(num != 0){
                addProduct(products[index], order, num);
            }
        }

    }while(!done);
    printTemporalOrder(order);
}

void endConnection(Connection c){
     int messageCode = 3;
     conn_send(c, &messageCode, sizeof(messageCode));
     //Esperar a que me responda?
     return;
}

void printProducts(Product *products, int num) {
    for(int i=0; i<num; i++){
        prettyPrintProduct(products[i]);
    }
}

void addProduct(Product product, Order order, int num){ //TODO checkear errores.
    addToOrder(order, getProductId(product), num, getProductPrice(product));
}

int requestProducts(Product ** products, int * numProducts, Connection c){ //TODO que los errores no sean prints
    
    int messageCode = 1;
    printf("Sending msgCode%d\n", messageCode );
    conn_send(c, &messageCode, sizeof(messageCode));
    void * serverResponse;
    size_t responseLength;
    int responseCode;
    conn_receive(c, &serverResponse, &responseLength);
    responseCode = *((int*)serverResponse);
    printf("El codigo de respuesta: %d\n",responseCode);

    if(responseCode < 0){
        printf("Error en la respuesta del pedido de productos");
        return -1;
    }
  
    conn_receive(c, &serverResponse, &responseLength);
    *numProducts = *((int*)serverResponse);
    printf("Cantidad de productos %d\n", *numProducts);

    *products = malloc(sizeof(*products) * (*numProducts));
    for(int i = 0; i<*numProducts; i++){
        conn_receive(c, &serverResponse, &responseLength);
        printf("responseLength: %d\n", responseLength);
        (*products)[i]=unserializeProduct(serverResponse);
    } 
    printf("Termina requestProducts\n");
    prettyPrintProduct(*products[0]);
    return 0;
}


void finishPurchase(Order order, Connection c){
    //TODO
    //Primero pido el address y despues mando la orden con el address.?? O pido el address primero?
    
    char * address= "TuCasa";
    /**
    printf("Whats your address? "); 
    scanf("%s", address);//Esto era super inseguro no? TODO hacer esto bien.
    */
    setAddress(order,address);
    
 

    int messageCode = 2;
    conn_send(c, &messageCode, sizeof(messageCode));

    printf("Todo Ok, antes de SerializarOrder\n");
    
    void * serializedOrder;
    size_t serializedOrderSize;
    serializedOrderSize=serializeOrder(order, &serializedOrder);
    printf("Todo Ok, antes de MandarOrder\n");
    conn_send(c,serializedOrder,serializedOrderSize);

    printf("Todo Ok\n");
    void * serverResponse;
    size_t responseLength;
    int responseCode;
    conn_receive(c, &serverResponse, &responseLength);
    responseCode = *((int*)serverResponse);
    printf("responseCode: %d\n", responseCode);

    if(responseCode < 0){ //Ver que hacer cuando esta mal el stock.
        printf("Error en la respuesta del envio de la Orden");
    }
  
}
