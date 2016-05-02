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
    printf("Connecting to server via %s...", getServerAddress());
    fflush(stdout);
    Connection c = conn_open(getServerAddress());
    printf("connected.\n");
    printf("Welcome to IAC (Inter-Alcohol Communication)!\n");
    int done = 0, option;
    Order order = malloc(sizeof(*order)); //Esto es el resumen de las ordenes. Maxima cantidad de diferentes productos es 16.
    Product *products;//Esto tiene los productos que le manda la base de dato. 
    int numProducts = 0; 
    requestProducts(products, &numProducts); //TODO deberia pedirle los productos al servidor.
    do {
        printProducts(products, numProducts); 
        option = scanInt("Use the numbers to select the product you would like to purchase\n Press 0 to Exit and 1 to Finish your purchase\n"); //TODO despues vemos la tecla y eso bien. Como hacer con mucho productos bla bla.
                
        switch(option) {
            case 0:
                done = 1;
                break;
            case 1:
                finishPurchase(order);
                //TODO que pasa si hay un error en la compra por desincronisacion? Avisar que hay algo mal y re imprimir.
                //No sale, sale con el 0. Esto es para confirmar compra. ???
                break;
            default:
                if(option > 0 && option <= numProducts) { //Because there are N products and 0 is exit. The products are the entries from 1 to N.
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
    int num = 0;
    //Verda:  
    
    do{
        prettyPrintProduct(products[index])
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
    printOrder(order);
}


void printProducts(Product *products, int num) {
    for(int i=0; i<num; i++){
        prettyPrintProduct(products[i]);
    }
}

void addProduct(Product product, Order order, int num){ //TODO checkear errores.
    addToOrder(order, getProductId(product), num);
}

void requestProducts(Product * products, int * numProducts){
    //TODO hablar con el server para requestiar los productolis
}

void finishPurchase(Order order){
    //TODO
    //Primero pido el adress y despues mando la orden con el adress.?? O pido el adress primero?
}

