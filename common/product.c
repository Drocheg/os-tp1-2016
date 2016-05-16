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

Product productNew(int id, const char *name, const char *description, const float price, const int stock) {
    Product result = malloc(sizeof (*result));
    size_t nameSize = strlen(name)+1,
            descrSize = strlen(description)+1;
    result->id = id;
    result->name = malloc(nameSize);
    strncpy(result->name, name, nameSize);
    result->description = malloc(descrSize);
    strncpy(result->description, description, descrSize);
    result->price = price;
    result->stock = stock;
    return result;
}

void productFree(Product p) {
    free(p->description);
    free(p->name);
    free(p);
}

void productPrint(Product p) {
    printf("*%s (#%i)*\n%s\nPrice: $%.2f\nStock: %i\n", productGetName(p), productGetId(p), productGetDescription(p), productGetPrice(p), productGetStock(p));
}

size_t productSerialize(const Product p, void **dest) {
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
    memcpy(*dest + offset, p->description, descrLen);
    offset += descrLen;
    memcpy(*dest + offset, &(p->price), sizeof (p->price));
    offset += sizeof (p->price);
    memcpy(*dest + offset, &(p->stock), sizeof (p->stock));
    return totalLen;
}

Product productUnserialize(const void* data) {
    Product result = malloc(sizeof (*result));
    int nameLen, descrLen, offset = 0;
    memcpy(&(result->id), data + offset, sizeof (result->id));
    offset += sizeof (result->id);
    memcpy(&nameLen, data + offset, sizeof (nameLen));
    offset += sizeof (nameLen);
    result->name = malloc(nameLen);
    memcpy(result->name, data + offset, nameLen);
    offset += nameLen;
    memcpy(&descrLen, data + offset, sizeof (descrLen));
    offset += sizeof (descrLen);
    result->description = malloc(descrLen);
    memcpy(result->description, data + offset, descrLen);
    offset += descrLen;
    memcpy(&(result->price), data + offset, sizeof (result->price));
    offset += sizeof (result->price);
    memcpy(&(result->stock), data + offset, sizeof (result->stock));
    return result;
}

int productGetId(Product p) {
    return p->id;
}

char *productGetName(Product p) {
    return p->name;
}

char *productGetDescription(Product p) {
    return p->description;
}

float productGetPrice(Product p) {
    return p->price;
}

int productGetStock(Product p) {
    return p->stock;
}

int productSetStock(Product p, int newStock) {
    if(newStock < 0) {
        return 0;
    }
    p->stock = newStock;
    return 1;
}