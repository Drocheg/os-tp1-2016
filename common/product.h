#ifndef PRODUCT_H
#define PRODUCT_H

#include <stddef.h>

typedef struct product_t* Product;

Product productNew(int id, const char *name, const char *description, const float price, const int stock);

void productFree(Product p);

void productPrint(Product p);

size_t productSerialize(const Product p, void **dest);

Product productUnserialize(const void* data);

int productGetId(Product p);

char *productGetName(Product p);

char *productGetDescription(Product p);

float productGetPrice(Product p);

int productGetStock(Product p);

/**
 * Sets the product stock.
 * 
 * @param p
 * @param newStock
 * @return 1 on success or 0 on error (i.e. negative stock specified)
 */
int productSetStock(Product p, int newStock);

#endif /* PRODUCT_H */

