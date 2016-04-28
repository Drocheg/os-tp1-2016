/* 
 * File:   main.c
 * Author: juan_
 *
 * Created on April 12, 2016, 9:54 PM
 */

#include <lib.h>
#include <stdio.h>
#include <unistd.h> // For sleeping
#include <config.h>
#include <comm.h>
#include <product.h>
#include <order.h>

 /**
Voy a hacer que le pida los productos al servidor, despues hace todo un sistema de compra 
donde se genera un "carro" de compras y al final se lo manda y el server lo checkea
El carro de compras va a ser una lista de ints en donde en cada nodo pongo el numero del producto y la cantidad.
Haria que sea un vector pero seria re poco escalable. Porque no es raro que un local tenga muchos productos distintos pero si es 
raro que un comprador compre muchos productos distintos. 
 **/

int main(int argc, char** argv) {
//    Product p = newProduct("Test", "Test product", 42.42, 42);
//    prettyPrintProduct(p);
//    char *s = serializeProduct(p);
//    printf("%.1s\n", s);
//    prettyPrintProduct(unserializeProduct(s));
//    return 0;
   

    printf("Connecting to server via %s...", getServerAddress());
    fflush(stdout);
    Connection c = conn_open(getServerAddress());
    printf("connected.\n");
    printf("Welcome to IAC (Inter-Alcohol Communication)!\n");
    int done = 0, option;
    Order order = malloc(sizeof(*order)); //Esto es el resumen de las ordenes
    Product *products;//Esto tiene los productos que le manda la base de dato. 
    int cantProducts = 0; 
    requestProducts(products, &cantProducts); //TODO deberia pedirle los productos al servidor.
    do {
        printProducts(products, cantProducts); //TODO imprime en pantalla los productos con sus stock y descripcion y bla bla.
        option = scanInt("Use the numbers to select the product you would like to purchase\n Press 0 to Exit and 1 to Finish your purchase\n"); //TODO despues vemos la tecla y eso bien. Como hacer con mucho productos bla bla.

        
        //option = scanInt("What would you like to do?\n\t1) Browse products\n\t2) Purchase selected products\n\t3) Exit\n\n");

        switch(option) {
            case 0:
                done = 1;
                break;
            case 1:
                finishPurchase(order);
                //TODO que pasa si hay un error en la compra por desincronisacion? Avisar que hay algo mal y re imprimir.
                done = 1;
                break;
            default:
                if(opcion>0 && opcion<=cantProducts) { //Because there are N products and 0 is exit. The products are the entries from 1 to N.
                    startPurchase(option-2); //Entrar en la etapa de comprar. Se pasa opcion-2 porque ese es el indice en el array.
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
    return 0;
}

void startPurchase(int index, Product *products, Order order) {
    int done = 0;
    int cant = 0;
    //Verda:
    
    do{
        printProduct(products[index])
        cant=scanInt("How many do you want to buy?\n Send 0 to cancel\n"); 
        if(cant<0 || cant>getProductStock(products[index])) {
            printf("Need to put a valid quantity");
        }
        else {
            done=1;
            if(cant>0) {
                addToOrder(products[index], order, cant);

            }
        }

    }while(!done)

}


void printProducts(Product *products, int cant) {
    for(int i=0; i<cant; i++){
        prettyPrintProduct(products[i]);
    }
}

void addToOrder(Product product, Order order, int cant){

}

void requestProducts(products, &cantProducts){
    //TODO
}

void finishPurchase(Order order){
    //TODO
}