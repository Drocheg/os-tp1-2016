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
int finishPurchase(Order * order, Connection c);
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


    Config config = setup();
    char *address = getServerAddress(config);

    printf("Connecting to server via %s...", address);
    fflush(stdout);
    Connection c = conn_open(address);
    if (c == NULL) {
        printf("Couldn't connect. Aborting.\n");
        return -1;
    }
    printf("connected.\n");
    printf("Welcome to IAC (Inter-Alcohol Communication)!\n");
    int done = 0, option;
    Order order = order_new(); //Esto es el resumen de las ordenes. Maxima cantidad de diferentes productos es 16.  
    Product *products; //Esto tiene los productos que le manda la base de dato. 
    int numProducts = -1;
    if(requestProducts(&products, &numProducts, c)==-1){
        printf("Error reading products.\n");
        done=1;
    }
    
    while(!done){
        printf("These are the available products:\n");
        printProducts(products, numProducts);
        option = scanInt("Use the numbers to select the product you would like to purchase\n Press 0 to Exit and 1 to Finish your purchase\n"); //TODO despues vemos la tecla y eso bien. Como hacer con mucho productos bla bla.
        if(option == 0) {
            done = 1;
            endConnection(c);
        }
        else if(option == 1) {
            if(finishPurchase(&order, c)==-1){
                printf("Error sending orden");
                done = 1;
            }
        }
        else if(option < 0 || option-2 >= numProducts) {
            printf("Invalid option selected.\n");
        }
        else {
            printf("Adding products not implemented yet.\n");
            startPurchase(option - 2, products, order); //Entrar en la etapa de comprar. Se pasa opcion-2 porque ese es el indice en el array.
        }
    };

    printf("Disconnecting...");
    fflush(stdout);
    conn_close(c);
    printf("done.\n");
    printf("See you next time!\n");
    order_free(order);
    return 0;
}

void startPurchase(int index, Product *products, Order order) {
    int done = 0;
    int num = 0;
    //Verda:  

    do {
        prettyPrintProduct(products[index]);
        num = scanInt("How many do you want to buy?\n Send 0 to cancel\n");
        if (num < 0 || num > getProductStock(products[index])) {
            printf("Need to put a valid quantity");
        } else {
            done = 1;
            if (num != 0) {
                addProduct(products[index], order, num);
            }
        }

    } while (!done);
    order_print(order);
}

void endConnection(Connection c) {
    int messageCode = MESSAGE_CLOSE;
    conn_send(c, &messageCode, sizeof (messageCode));
    //Esperar a que me responda?
    return;
}

void printProducts(Product *products, int num) {
    for (int i = 0; i < num; i++) {
        printf("%i - ", i+2);
        prettyPrintProduct(products[i]);
    }
}

void addProduct(Product product, Order order, int num) { //TODO checkear errores.
    order_add(order, getProductId(product), num, getProductPrice(product));
}

int requestProducts(Product ** products, int * numProducts, Connection c) {
    int messageCode = CMD_GET_PRODUCTS;
    conn_send(c, &messageCode, sizeof (messageCode));
    void * serverResponse;
    size_t responseLength;
    int responseCode;
    conn_receive(c, &serverResponse, &responseLength);
    responseCode = *((int*) serverResponse);
    if (responseCode == MESSAGE_ERROR) {
//        log_warn("Client received error code from server when requesting products.");
        return -1;
    }
    *numProducts = responseCode;
    *products = malloc(*numProducts);
    for (int i = 0; i < *numProducts; i++) {
        size_t serializedLen;
        void* serialized = malloc(serializedLen);
        conn_receive(c, &serialized, &serializedLen);
        (*products)[i] = unserializeProduct(serialized);
        free(serialized);
    }
    return 0;
}

int finishPurchase(Order * order, Connection c) {
    //TODO
    //Primero pido el address y despues mando la orden con el address.?? O pido el address primero?

    char * address = "TuCasa";
    /**
    printf("Whats your address? "); 
    scanf("%s", address);//Esto era super inseguro no? TODO hacer esto bien.
     */
    order_set_addr(*order, address);



    int messageCode = CMD_PLACE_ORDER;
    conn_send(c, &messageCode, sizeof (messageCode));

    printf("Todo Ok, antes de SerializarOrder\n");

    void * serializedOrder;
    size_t serializedOrderSize;
    serializedOrderSize = order_serialize(*order, &serializedOrder);
    
    conn_send(c, serializedOrder, serializedOrderSize);

    void * serverResponse;
    size_t responseLength;
    int responseCode;
    conn_receive(c, &serverResponse, &responseLength);
    responseCode = *((int*) serverResponse);
    printf("responseCode: %d\n", responseCode); //Testing, borrar

    if (responseCode == MESSAGE_ERROR) { //Ver que hacer cuando esta mal el stock.
        return -1;
    }
    if(responseCode == MESSAGE_UNSATISFIABLE_ORDER){
        printf("Error, not enough stock. Your order has been changed to the new stock.\n New Order: \n");
        conn_receive(c, &serializedOrder, &serializedOrderSize);
        order_free(*order);
        *order = order_unserialize(serializedOrder);
        order_print(order);
    }
}
