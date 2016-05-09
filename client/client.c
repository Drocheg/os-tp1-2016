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
#include <request.h>
#include <data.h>

void startPurchase(int index, Product *products, Order order);
void finishPurchase(Order order);
void printProducts(Product *products, int num);
void addProduct(Product product, Order order, int num);
void requestProducts(Product * products, int * numProducts, Connection c);

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
    requestProducts(products, &numProducts, c); //TODO deberia pedirle los productos al servidor.
    do {
        printProducts(products, numProducts); 
        option = scanInt("Use the numbers to select the product you would like to purchase\n Press 0 to Exit and 1 to Finish your purchase\n"); //TODO despues vemos la tecla y eso bien. Como hacer con mucho productos bla bla.
                
        switch(option) {
            case 0:
                done = 1;
                break;
            case 1:
                finishPurchase(order, c);
                //TODO que pasa si hay un error en la compra por desincronisacion? Avisar que hay algo mal y re imprimir.
                //No sale, sale con el 0. Esto es para confirmar compra. ???
                break;
            default:
                if(option > 0 && option <= numProducts) { //Because there are N products and 0 is exit. The products are the entries from 1 to N.
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


void printProducts(Product *products, int num) {
    for(int i=0; i<num; i++){
        prettyPrintProduct(products[i]);
    }
}

void addProduct(Product product, Order order, int num){ //TODO checkear errores.
    addToOrder(order, getProductId(product), num);
}

/**

void requestProducts(Product * products, int * numProducts){ //TODO que los errores no sean prints
    Request requestProducts = createRequest(1); //TODO
    Response responseProducts = request(c, requestProducts); //TODO TODO
    if(getResponseError(responseProducts) != 0){
        printf("Error en la request de productos");
        return -1;
    }

    int argc = getArgc(responseProducts);
    if(argc!=2){
        printf("Cantidad erronea de argumentos en requestProducts");
        return -1;
    }

    Argument * arguments = getArguments(responseProducts);

    Datatype data1 =getDataType(arguments[0]);
    if(data1!=INT){
        printf("El primer argumento de requestProducts deberia ser un int");
        return -1;
    }

    Datatype data2 =getDataType(arguments[1]);
    if(data2!=PRODUCTS){
        printf("El segundo argumento de requestProducts deberia ser un products");
        return -1;
    }
    
    numProducts = (int)getArg(arguments[0]);
    products = (Product *) getArg(arguments[1]);

    return 0;
    

}

*/


void requestProducts(Product * products, int * numProducts, Connection c){ //TODO que los errores no sean prints
    Message messageProducts = createMessage(1); //TODO
    sendMessage(c, messageProducts); //TODO TODO message devuelve un msj? No deberia.
    Message responseProducts = listenMessage(c);
    if(getMessageCode(responseProducts) < 0){
        printf("Error en la orden de productos");
        return -1;
    }

    int argc = getArgc(responseProducts);
    if(argc!=2){
        printf("Cantidad erronea de argumentos en el message de respuesta del servidor en messageProducts");
        return -1;
    }

    Argument * arguments = getArguments(responseProducts);

    Datatype data1 =getDataType(arguments[0]);
    if(data1!=INT){
        printf("El primer argumento de messageProducts deberia ser un int");
        return -1;
    }

    Datatype data2 =getDataType(arguments[1]);
    if(data2!=PRODUCTS){
        printf("El segundo argumento de messageProducts deberia ser un products");
        return -1;
    }
    
    numProducts = (int)getArg(arguments[0]);
    products = (Product *) getArg(arguments[1]);

    return 0;
    

}


void finishPurchase(Order order, Connection c){
    //TODO
    //Primero pido el address y despues mando la orden con el address.?? O pido el address primero?
    char * address;
    printf("Whats your address? "); 
    scanf("%s", address);//Esto era super inseguro no? TODO hacer esto bien.

    order->address = address; //Puedo hacer esto fuera de order.c? TODO crear una funcion en order.c

    Message messageOrder = createMessage(2);
    Data data = newData(order, sizeof(*order), ORDER);
    addParam(messageOrder, data, sizeof(*order), order); //No tiene mucho sentido. La info esta en data o en addParam? Creo que algo quedo viejo.
    sendMessage(c, messageOrder);

    Message responseOrder = listenMessage(c);
    int code = getMessageCode(responseOrder); 
    if(code==0){
        printf("Todo OK en finish purchase\n");
    }else{
        printf("No se puedo completar la compra\n");
    }
}

