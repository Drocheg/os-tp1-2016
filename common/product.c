#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <lib.h>
#include <stdio.h>

#include "product.h"

struct product_t {
    int id;
    char *name;
    char *description;
    float price;
    int stock;
};

Product newProduct(int id, const char *name, const char *description, const float price, const int stock) {
    Product result = malloc(sizeof (*result));
    result->id = id;
    result->name = malloc(strlen(name) + 1);
    strcpy(result->name, name); // TODO NetBeans suggests using strncpy to prevent buffer overflow
    result->description = malloc(strlen(description) + 1);
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
    printf("*%s (#%i)*\n%s\nPrice: $%.2f\nStock: %i\n", getProductName(p), getProductId(p), getProductDescription(p), getProductPrice(p), getProductStock(p));
}

size_t serializeProduct(const Product p, void **dest) {
    //TODO check for errors and return NULL
    int offset = 0;
    int nameLen = strlen(p->name) + 1;
    int descrLen = strlen(p->description) + 1;
    size_t totalLen = sizeof (p->id) + sizeof (nameLen) + nameLen + sizeof (descrLen) + descrLen + sizeof (p->price) + sizeof (p->stock);
    *dest = malloc(totalLen);
    memcpy(*dest + offset, &(p->id), sizeof (p->id));
    offset += sizeof (p->id);
    memcpy(*dest + offset, &nameLen, sizeof (nameLen));
    offset += sizeof (nameLen);
    memcpy(*dest + offset, p->name, nameLen);
    offset += nameLen;
    memcpy(*dest + offset, &descrLen, sizeof (descrLen));
    offset += sizeof (descrLen);
    strcpy(*dest + offset, p->description);
    offset += descrLen;
    memcpy(*dest + offset, &(p->price), sizeof (p->price));
    offset += sizeof (p->price);
    memcpy(*dest + offset, &(p->stock), sizeof (p->stock));
    return totalLen;
}

Product unserializeProduct(const void* data) {
    //TODO check for errors and return NULL
    Product result = malloc(sizeof (*result));
    int nameLen, descrLen, offset = 0;
    memcpy(&(result->id), data + offset, sizeof (result->id));
    offset += sizeof (result->id);
    memcpy(&nameLen, data + offset, sizeof (nameLen));
    offset += sizeof (nameLen);
    result->name = malloc(nameLen);
    strcpy(result->name, data + offset);
    offset += nameLen;
    memcpy(&descrLen, data + offset, sizeof (descrLen));
    offset += sizeof (descrLen);
    result->description = malloc(descrLen);
    strcpy(result->description, data + offset);
    offset += descrLen;
    memcpy(&(result->price), data + offset, sizeof (result->price));
    offset += sizeof (result->price);
    memcpy(&(result->stock), data + offset, sizeof (result->stock));
    return result;
}

int getProductId(Product p) {
    return p->id;
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

int setProductStock(Product p, int newStock) {
    if(newStock < 0) {
        return 0;
    }
    p->stock = newStock;
    return 1;
}