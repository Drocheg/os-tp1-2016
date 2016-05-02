#ifndef order_h
#define order_h

typedef struct order_t * Order; //Maxima cantidad de diferentes productos es 16.

int getCantEntries();

int addToOrder(Order order, int product_id, int quantity);

void print(Order order);

#endif /* order_h */