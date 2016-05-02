#ifndef order_h
#define order_h

#include <stdlib.h>
#include <stdio.h>
typedef struct order_t * Order; //Maxima cantidad de diferentes productos es 16.

int getNumEntries();

int addToOrder(Order order, int product_id, int quantity);

void printTemporalOrder(Order order);

Order newOrder();

void freeOrder(Order order);

#endif /* order_h */