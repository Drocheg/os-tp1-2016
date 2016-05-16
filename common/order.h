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
OrderEntry orderGetEntryNum(Order o, int entryNum);

/**
 * Gets the product ID of the specified order entry.
 * 
 * @param e
 * @return 
 */
int orderEntryGetId(OrderEntry e);

/**
 * Gets the price of the product in the specified order entry.
 * 
 * @param e
 * @return 
 */
float orderEntryGetPrice(OrderEntry e);

/**
 * Gets the quantity of the product in the specified order entry.
 * 
 * @param e
 * @return 
 */
int orderEntryGetQuantity(OrderEntry e);

/**
 * Updates the quantity of the specified order entry to the new quantity.
 * 
 * @param e The order entry to modify.
 * @param newQuantity The new quantity.
 */
void orderEntrySetQuantity(OrderEntry e, int newQuantity);

/**
 * Gets the number of entries in the specified order.
 * @return 
 */
int orderGetNumEntries(Order o);

/**
 * Gets the total of the specified order.
 * 
 * @param o
 * @return 
 */
float orderGetTotal(Order o);

/**
 * Gets the address associated with this order.
 * 
 * @param order
 * @return The address, or NULL if none is saved.
 */
char* orderGetAddress(Order order);

/**
 * Sets the delivery address in the specified order.
 * 
 * @param order
 * @param address
 */
void orderSetAddress(Order order, char * address);


/**
 * Adds the specified quantity of the specified product to the specified entry.
 * If an entry already exists for this product, will update that entry rather
 * than creating a new one.
 * 
 * @param order
 * @param product_id
 * @param quantity
 * @param price
 * @return 0 on success, -1 if this order can't have any more entries.
 */
int orderAdd(Order order, int product_id, int quantity, float price);


/**
 * Pretty-prints the specified order.
 * 
 * @param order
 */
void orderPrint(Order order);

/**
 * Creates a new empty order.
 * 
 * @return 
 */
Order orderNew();

/**
 * Frees resources associated with the specified order.
 * 
 * @param order
 */
void orderFree(Order order);

/**
 * Serializes the specified order in the specified destination byte array.
 * 
 * @param o The order to serialize.
 * @param dest Where to serialize it.
 * @return The size (in bytes) of the serialized order.
 */
size_t orderSerialize(const Order o, void **dest);

/**
 * Unserializes a serialized order.
 * 
 * @param data The serialized order.
 * @return The unserialized order.
 */
Order orderUnserialize(const void* data);

#endif /* order_h */