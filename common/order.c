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
    OrderEntry *items;
};

OrderEntry orderGetEntryNum(Order o, int entryNum) {
    if (entryNum < 0 || entryNum >= orderGetNumEntries(o)) {
        return NULL;
    }
    return o->items[entryNum];
}

int orderEntryGetId(OrderEntry e) {
    return e->product_id;
}

void orderEntrySetQuantity(OrderEntry e, int newQuantity) {
    e->quantity = newQuantity;
}

int orderEntryGetQuantity(OrderEntry e) {
    return e->quantity;
}

float orderEntryGetPrice(OrderEntry e) {
    return e->price;
}

Order orderNew() {
    Order order = malloc(sizeof (*order));
    order->numEntries = 0;
    order->address = NULL;
    order->items = calloc(ORDER_MAX_SIZE, sizeof(*(order->items)));
    return order;
}

void orderFree(Order o) { //TODO free de address? Cuando le doy espacio? 
    for (int i = 0; i < o->numEntries; i++) {
        if(o->items[i] != NULL) {
            free(o->items[i]);
        }
    }
    free(o);
}

float orderGetTotal(Order order) {
    float result = 0;
    for (int i = 0; i < order->numEntries; i++) {
        result += order->items[i]->price * order->items[i]->quantity;
    }
    return result;
}

char* orderGetAddress(Order order) {
    return order->address;
}

void orderSetAddress(Order order, char * address) {
    if(order->address != NULL) {
        free(order->address);
    }
    if(address == NULL) {
        order->address = NULL;
    }
    else {
        int len = strlen(address)+1;
        order->address = malloc(len);
        strncpy(order->address, address, len);
    }
}

int orderGetNumEntries(Order order) {
    return order->numEntries;
}

int orderAdd(Order order, int product_id, int quantity, float price) {
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

size_t orderSerialize(const Order o, void **dest) {
    int offset = 0;
    size_t addressLen = o->address == NULL ? 0 : strlen(o->address) + 1;
    size_t totalLen = sizeof (addressLen) + addressLen + sizeof(o->numEntries) + (o->numEntries * sizeof(struct order_entry_t));
    *dest = malloc(totalLen);
    memcpy(*dest + offset, &addressLen, sizeof (addressLen));
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

Order orderUnserialize(const void* data) {
    Order result = orderNew();
    size_t addressLen = -1;
    int offset = 0;
    memcpy(&addressLen, data + offset, sizeof (addressLen));
    offset += sizeof (addressLen);
    if(addressLen > 0) {
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

void orderPrint(Order order) {
    printf("-----------------------------------------------------\n");
    if(order->numEntries == 0) {
        printf("Empty order (we require more booze!)\n");
    }
    else {
        for (int i = 0; i < order->numEntries; i++) {
            printf("%i of %i\n\tProduct #%i, unit price $%.2f, quantity %i\n", i+1, order->numEntries, order->items[i]->product_id, order->items[i]->price, order->items[i]->quantity);
        }
        printf("GRAND TOTAL: $%.2f\n", orderGetTotal(order));
    }
    printf("-----------------------------------------------------\n");
}
