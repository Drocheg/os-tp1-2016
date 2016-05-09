#ifndef order_h
#define order_h

#include "product.h"

typedef struct order_t * Order; //Maxima cantidad de diferentes productos es 16.

int getNumEntries();

void setAddress(Order order, char * address);

int addToOrder(Order order, int product_id, int quantity);

void printTemporalOrder(Order order);

Order newOrder();

void freeOrder(Order order);

int serializeOrder(const Order o, void **dest);

Order unserializeOrder(const void* data);

#endif /* order_h */