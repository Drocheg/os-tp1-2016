#include "order.h"

struct orderEntry_t{
	int product_id;
	int quantity;
};

struct order_t {
	char * address; //Cuando malloc? Supongo que en el momento de agregarle el address.
	int numEntries; 
	struct orderEntry_t items[16]; //Se podria hacer dinamico	
};


Order newOrder(){
	Order order = malloc(sizeof(*order)); 
	order->numEntries = 0;
	return order;
}

void freeOrder(Order order){
	free(order);
}

//float getTotalPrice();
int getNumEntries(Order order){ //Me confundi no se si sirve esto.
	return order->numEntries;
}

int addToOrder(Order order, int product_id, int quantity){
	int i=0;
	for (i=0; i<order->numEntries; i++){
		if(product_id == order->items[i].product_id){
			break;
		}
	}
	if(i<order->numEntries){
		order->items[i].quantity += quantity;
	}else{
		order->numEntries++;
		order->items[i].product_id= product_id; 
		order->items[i].quantity  = quantity;
	}
	return 0;
}


int serializeOrder(const Order o, void **dest) {
    //TODO check for errors and return NULL
    int offset = 0;
    int addressLen = strlen(o->address)+1;
    int totalLen = sizeof(addressLen) + addressLen + sizeof(o->numEntries) + sizeof(o->items);
    *dest = malloc(totalLen);
    memcpy(*dest, &addressLen, sizeof(addressLen));
    offset += sizeof(addressLen);
    memcpy(*dest+offset, o->address, addressLen);
    offset += addressLen;
    memcpy(*dest+offset, &(o->numEntries), sizeof(o->numEntries));
    offset += sizeof(o->numEntries);
    memcpy(*dest+offset, &(o->items), sizeof(o->items));
    return 1;
}

Product unserializeOrder(const void* data) {
    //TODO check for errors and return NULL
    Order order = malloc(sizeof(*order));
    int addressLen, offset = 0;
    memcpy(&addressLen, data, sizeof(addressLen));
    offset += sizeof(addressLen);
    order->address = malloc(addressLen);
    strcpy(order->address, data+offset);
    offset += addressLen;
    memcpy(&(order->numEntries), data+offset, sizeof(order->numEntries));
    offset += sizeof(order->numEntries);
    memcpy(&(order->items), data+offset, sizeof(order->items));
    return result;
}

void printTemporalOrder(Order order){ //TODO Borrar o hacer bien
	for(int i=0; i<order->numEntries; i++){
		printf("%d\n", order->items[i].product_id);
		printf("%d\n", order->items[i].quantity);
	}
}
