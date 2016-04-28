#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <lib.h>
#include <stdio.h>

#include "product.h"

struct product_t {
    char *name;
    char *description;
    float price;
    int stock;
};

Product newProduct(const char *name, const char *description, const float price, const int stock) {
    Product result = malloc(sizeof(*result));
    result->name = malloc(strlen(name)+1);
    strcpy(result->name, name); // TODO NetBeans suggests using strncpy to prevent buffer overflow
    result->description = malloc(strlen(description)+1);
    strcpy(result->description, description); // TODO NetBeans suggests using strncpy to prevent buffer overflow
    result->price = price;
    result->stock = stock;
    return result;
}

void freeProduct(Product p) {
    free(p->description);
    free(p->name);
    free(p);
}

void prettyPrintProduct(Product p) {
    printf("*%s*\n%s\nPrice: $%f\nStock: %i\n", getProductName(p), getProductDescription(p), getProductPrice(p), getProductStock(p));
}

/**
 * 1) strlen(name)
 * 2) name
 * 3) strlen(description)
 * 4) description
 * 5) price (float)
 * 6) stock (int)
 * @param p
 * @return 
 */
char* serializeProduct(const Product p) {
    //TODO check for errors and return NULL
    int offset = 0;
    int nameLen = strlen(p->name)+1;
    int descrLen = strlen(p->description)+1;
    int totalLen = sizeof(nameLen) + nameLen + sizeof(descrLen) + descrLen + sizeof(p->price) + sizeof(p->stock);
    char * result = malloc(totalLen);
    memcpy(result, &nameLen, sizeof(nameLen));
    offset += sizeof(nameLen);
    strcpy(result+offset, p->name);
    offset += nameLen;
    memcpy(result+offset, &descrLen, sizeof(descrLen));
    offset += sizeof(descrLen);
    strcpy(result+offset, p->description);
    offset += descrLen;
    memcpy(result+offset, &(p->price), sizeof(p->price));
    offset += sizeof(p->price);
    memcpy(result+offset, &(p->stock), sizeof(p->stock));
    return result;
}

Product unserializeProduct(const char* data) {
    //TODO check for errors and return NULL
    Product result = malloc(sizeof(*result));
    int nameLen, descrLen, offset = 0;
    memcpy(&nameLen, data, sizeof(nameLen));
    offset += sizeof(nameLen);
    result->name = malloc(nameLen);
    strcpy(result->name, data+offset);
    offset += nameLen;
    memcpy(&descrLen, data+offset, sizeof(descrLen));
    offset += sizeof(descrLen);
    result->description = malloc(descrLen);
    strcpy(result->description, data+offset);
    offset += descrLen;
    memcpy(&(result->price), data+offset, sizeof(result->price));
    offset += sizeof(result->price);
    memcpy(&(result->stock), data+offset, sizeof(result->stock));
    return result;
}

char *getProductName(Product p) {
    return p->name;
}

char *getProductDescription(Product p) {
    return p->description;
}

float getProductPrice(Product p) {
    return p->price;
}

int getProductStock(Product p) {
    return p->stock;
}