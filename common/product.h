/* 
 * File:   product.h
 * Author: jlipumafinnemore
 *
 * Created on 26 de abril de 2016, 10:59
 */

#ifndef PRODUCT_H
#define PRODUCT_H

#include <stddef.h>

typedef struct product_t* Product;

Product newProduct(const char *name, const char *description, const float price, const int stock);

void freeProduct(Product p);

void prettyPrintProduct(Product p);

size_t serializeProduct(const Product p, void **dest);

Product unserializeProduct(const void* data);

int getProductId(Product p);

char *getProductName(Product p);

char *getProductDescription(Product p);

float getProductPrice(Product p);

int getProductStock(Product p);

#endif /* PRODUCT_H */

