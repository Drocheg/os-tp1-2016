#include <string.h>

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

void prettyPrintProduct(Product p) {
    printf("*%s*\n%s\nPrice: $%f\nStock: %i\n", getProductName(p), getProductDescription(p), getProductPrice(p), getProductStock(p));
}