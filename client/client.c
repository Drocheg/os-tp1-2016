#include <lib.h>
#include <config.h>
#include <comm.h>
#include <product.h>
#include <order.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>

Connection conn;

/**
 * Adds the product in the specified array to the specified order, from the
 * specified product array.
 * 
 * @param index Product number. <b>NOTE: </b>0-indexed.
 * @param products The product array.
 * @param order Where to add the product to.
 */
void addToOrder(int index, Product *products, Order order);

/**
 * Places the specified order, requesting validation from the server through the
 * client's connectino.
 * 
 * @param order The order to place.
 * @return 1 on success, 0 if the order couldn't be satisfied (i.e. not enough
 * stock) or -1 on errror.
 */
int submitOrder(Order *order);

/**
 * Requests products from the server through the specified connection, storing
 * the result in the specified product array, and updating *numProducts to show
 * the number of fetched products.
 * 
 * @param products Where to store the received products.
 * @param numProducts Where to store the number of received products.
 * @return 1 on success, -1 on error.
 */
int requestProducts(Product ** products, int * numProducts);

void printProducts(Product *products, int num);
void addProduct(Product product, Order order, int num);
void disconnect();
void shutDown(int signo);

int main(int argc, char** argv) {
    Config config = setup();
    char *address = getServerAddress(config);

    printf("Connecting to server via %s...", address);
    fflush(stdout);
    conn = conn_open(address);
    if (conn == NULL) {
        printf("couldn't connect. Aborting.\n");
        return -1;
    }
    printf("connected.\n");
    signal(SIGINT, shutDown);
    printf("Welcome to IAC (Inter-Alcohol Communication)!\n");
    int done = 0, option;
    Order order = orderNew();
    Product *products;
    int numProducts = -1;
    if (requestProducts(&products, &numProducts) == -1) {
        printf("Error fetching products. Aborting.\n");
        done = 1;
    }
    while (!done) {
        option = scanInt("What would you like to do?\n1 - View available products\n2 - Add a product\n3 - View your order\n4 - Submit your order\n0 - Quit\n");
        switch (option) {
            case 1:
                printf("These are the available products:\n");
                printProducts(products, numProducts);
                break;
            case 2:
                do {
                    option = scanInt("Which product number would you like to add to your order?\nEnter 0 to cancel\n");
                    if(option < 0 || option > numProducts){
                        printf("Invalid product number\n");
                    }
                } while (option < 0 || option > numProducts);
                if (option != 0) {
                    addToOrder(option - 1, products, order);
                }
                break;
            case 3:
                orderPrint(order);
                break;
            case 4:
                if(orderGetNumEntries(order) == 0) {
                    printf("Your order is empty, please add items to your order.\n");
                    break;
                }
                switch (submitOrder(&order)) {
                    case -1:
                        printf("Error processing order, aborting.\n");
                        done = 1;
                        break;
                    case 0:
                        printf("Sorry, stock changed while you were browsing and your order can't be fulfilled.\nThis is the biggest order we can complete:\n");
                        orderPrint(order);
                        break;
                    case 1:
                        printf("Order placed. Thank you for your order!\n");
                        done = 1;
                        break;
                }
                break;
            case 0:
                done = 1;
                break;
            default:
                printf("Invalid option selected.\n");
                break;
        }
    };
    printf("Disconnecting...");
    fflush(stdout);
    disconnect();
    printf("disconnected.\n");
    printf("See you next time!\n");
    orderFree(order);
    return 0;
}

void addToOrder(int index, Product *products, Order order) {
    int done = 0;
    int num = 0;
    int availStock = productGetStock(products[index]);
    do {
        productPrint(products[index]);
        num = scanInt("How many of this product would you like? Enter 0 to cancel\n");
        if (num < 0 || num > availStock) {
            printf("Please enter a valid amount.\n");
        } else {
            done = 1;
            if (num != 0) {
                int id = productGetId(products[index]);
                float price = productGetPrice(products[index]);
                if (orderAdd(order, id, num, price) == -1) {
                    printf("Sorry, you can only have up to 16 different products in your order.\nTry making a separate order for more products!\n");
                } else {
                    productSetStock(products[index], availStock-num);
                    printf("Added %i %s to your order, %.2f each.\n", num, productGetName(products[index]), productGetPrice(products[index]));
                }
            }
        }

    } while (!done);
}

void shutDown(int signo) {
    if (signo != SIGINT)
        return;

    printf("Received SIGINT, shutting down.\n");
    disconnect();
    exit(0);
}

void disconnect() {
    int messageCode = MESSAGE_CLOSE;
    conn_send(conn, &messageCode, sizeof (messageCode));
    conn_close(conn);
    conn = NULL;
    return;
}

void printProducts(Product *products, int numProducts) {
    for (int i = 0; i < numProducts; i++) {
        printf("%i - ", i + 1);
        productPrint(products[i]);
    }
}

int requestProducts(Product ** products, int * numProducts) {
    int messageCode = CMD_GET_PRODUCTS;
    conn_send(conn, &messageCode, sizeof (messageCode));
    void * serverResponse;
    size_t responseLength;
    int responseCode;
    conn_receive(conn, &serverResponse, &responseLength);
    responseCode = *((int*) serverResponse);
    if (responseCode == MESSAGE_ERROR) {
        return -1;
    }
    *numProducts = responseCode;
    *products = malloc(*numProducts);
    for (int i = 0; i < *numProducts; i++) {
        size_t serializedLen;
        void* serialized = malloc(serializedLen);
        conn_receive(conn, &serialized, &serializedLen);
        (*products)[i] = productUnserialize(serialized);
        free(serialized);
    }
    return 1;
}

int submitOrder(Order *order) {
    fflush(stdout);
    char address[1024];
    int i, c, valid = 0;
    //Accept only alphanumerical characters in address so as to not break database.
    while(!valid) {
        printf("Please enter your delivery address (alphanumeric characters only): ");
        i = 0;
        while (i < sizeof (address) - 1 && (c = getchar()) != '\n') {
            if(!isalnum(c) && c != ' ') {
                break;
            }
            address[i++] = c;
        }
        if(c == '\n') {    //Reached end of line with all valid characters
            valid = 1;
        }
        else {
            while(getchar() != '\n');   //Consume rest of entry
        }
    }
    address[i] = 0;
    orderSetAddress(*order, address);

    //Order ready, place order
    int messageCode = CMD_PLACE_ORDER;
    conn_send(conn, &messageCode, sizeof (messageCode)); //TODO abort if these fail
    void * buff;
    size_t buffSize;
    buffSize = orderSerialize(*order, &buff);
    conn_send(conn, buff, buffSize);
    free(buff);
    //Order sent, parse response
    int responseCode;
    conn_receive(conn, &buff, NULL); //The first part of the response will be an int, ignore length
    responseCode = *((int*) buff);
    free(buff);
    if (responseCode == MESSAGE_ERROR) { //TODO abort?
        return -1;
    }
    else if (responseCode == MESSAGE_UNSATISFIABLE_ORDER) {
        conn_receive(conn, &buff, &buffSize);
        orderFree(*order);
        *order = orderUnserialize(buff);
        return 0;
    } else {
        return 1;
    }
}