#include "order.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct order_entry_t {
    int product_id;
    int quantity;
    float price;
};

struct order_t {
    char * address;
    int numEntries;
    OrderEntry items[ORDER_MAX_SIZE]; //TODO hacer dinamico o que no te deje agregar más el cliente.
};

OrderEntry order_get_entry(Order o, int entryNum) {
    if (entryNum <= 0 || entryNum >= order_get_num_entries(o)) {
        return NULL;
    }
    return o->items[entryNum];
}

int orderentry_get_id(OrderEntry e) {
    return e->product_id;
}

void orderentry_set_quantity(OrderEntry e, int newQuantity) {
    e->quantity = newQuantity;
}

int orderentry_get_quantity(OrderEntry e) {
    return e->quantity;
}

float orderentry_price(OrderEntry e) {
    return e->price;
}

Order order_new() {
    Order order = malloc(sizeof (*order));
    order->numEntries = 0;
    order->address = NULL;
    return order;
}

void order_free(Order o) { //TODO free de address? Cuando le doy espacio? 
    for (int i = 0; i < o->numEntries; i++) {
        free(o->items[i]);
    }
    free(o);
}

float order_get_total(Order order) {
    float result = 0;
    for (int i = 0; i < order->numEntries; i++) {
        result += 1/*ACA PRECIO*/ * order->items[i]->quantity;   //Aca Diego pone el precio de cada item
    }
    return result;
}

char* order_get_addr(Order order) {
    return order->address;
}

void order_set_addr(Order order, char * address) {
    order->address = address;
}

int order_get_num_entries(Order order) {
    return order->numEntries;
}

int order_add(Order order, int product_id, int quantity, float price) {
    int i = 0;
    for (i = 0; i < order->numEntries; i++) {
        if (product_id == order->items[i]->product_id) {
            break;
        }
    }
    if (i < order->numEntries) {
        order->items[i]->quantity += quantity;
    } else if (i < ORDER_MAX_SIZE) { 
        order->numEntries++;
        order->items[i] = malloc(sizeof (*(order->items[i])));
        order->items[i]->product_id = product_id;
        order->items[i]->quantity = quantity;
        order->items[i]->price = price;
    } else {
        return -1;
    }
    return 0;
}

size_t order_serialize(const Order o, void **dest) {
    //TODO check for errors and return NULL
    int offset = 0;
    size_t addressLen = o->address == NULL ? 0 : strlen(o->address) + 1;
    size_t totalLen = sizeof (addressLen) + addressLen + sizeof (o->numEntries) + (sizeof (o->items) * sizeof (*(o->items[0])));    //TODO verify sum is correct
    *dest = malloc(totalLen);
    memcpy(*dest, &addressLen, sizeof (addressLen));
    offset += sizeof (addressLen);
    if(addressLen > 0) {
        memcpy(*dest + offset, o->address, addressLen);
    }
    offset += addressLen;
    memcpy(*dest + offset, &(o->numEntries), sizeof (o->numEntries));
    offset += sizeof (o->numEntries);
    for(int i = 0; i < o->numEntries; i++) {
        memcpy(*dest+offset, &(o->items[i]->product_id), sizeof(o->items[i]->product_id));
        offset += sizeof(o->items[i]->product_id);
        memcpy(*dest+offset, &(o->items[i]->quantity), sizeof(o->items[i]->quantity));
        offset += sizeof(o->items[i]->quantity);
        memcpy(*dest+offset, &(o->items[i]->price), sizeof(o->items[i]->price));
        offset += sizeof(o->items[i]->price);
    }
    return totalLen;
}

Order order_unserialize(const void* data) {
    //TODO check for errors and return NULL
    Order result = malloc(sizeof (*result));
    size_t addressLen;
    int offset = 0;
    memcpy(&addressLen, data, sizeof (addressLen));
    offset += sizeof (addressLen);
    if(addressLen == 0) {
        result->address = NULL;
    }
    else {
        result->address = malloc(addressLen);
        strncpy(result->address, data + offset, addressLen);
    }
    offset += addressLen;
    memcpy(&(result->numEntries), data + offset, sizeof (result->numEntries));
    offset += sizeof (result->numEntries);
    for(int i = 0; i < result->numEntries; i++) {
        result->items[i] = malloc(sizeof(*(result->items[i])));
        memcpy(&(result->items[i]->product_id), data+offset, sizeof(result->items[i]->product_id));
        offset += sizeof(result->items[i]->product_id);
        memcpy(&(result->items[i]->quantity), data+offset, sizeof(result->items[i]->quantity));
        offset += sizeof(result->items[i]->quantity);
        memcpy(&(result->items[i]->price), data+offset, sizeof(result->items[i]->price));
        offset += sizeof(result->items[i]->price);
    }
    return result;
}

void order_print(Order order) { //TODO Borrar o hacer bien
    for (int i = 0; i < order->numEntries; i++) {
        printf("%i of %i - %i x (id: %i price: %f) \n", i+1, order->numEntries, order->items[i]->quantity, order->items[i]->product_id, order->items[i]->price);
    }
}
