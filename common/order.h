#ifndef order_h
#define order_h

#include "product.h"

#define ORDER_MAX_SIZE 16

typedef struct order_t * Order; //Maxima cantidad de diferentes productos es 16.
typedef struct order_entry_t* OrderEntry;

/**
 * Gets the specified entry number of the specified order, if present.
 * 
 * @param o The order to get the entry from.
 * @param entryNum The entry number, 0-indexed.
 * @return The order entry or NULL if out of bounds.
 */
OrderEntry order_get_entry(Order o, int entryNum);

int orderentry_id(OrderEntry e);

int orderentry_quantity(OrderEntry e);

int order_get_num_entries();

void order_set_addr(Order order, char * address);

int order_add(Order order, int product_id, int quantity);

void order_print(Order order);

Order order_new();

void order_free(Order order);

size_t order_serialize(const Order o, void **dest);

Order order_unserialize(const void* data);

#endif /* order_h */