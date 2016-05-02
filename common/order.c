#include "order.h"

struct order_t {
	orderEntry_t[16] items;
	char * address;
	int numEntries; 
};

struct order_entry_t{
	int product_id;
	int quantity;
};


//float getTotalPrice();
int getNumEntries(Order order){ //Me confundi no se si sirve esto.
	return order->numEntries;
}

int addToOrder(Order order, int product_id, int quantity){
	for (int i=0; i<order->numEntries; i++){
		if(product_id = order->items[i].product_id){
			break;
		}
	}
	if(i<numEntries){
		order->items[i].quantity += quantity;
	}else{
		numEntries++;
		order->items[i].product_id= product_id; 
		order->items[i].quantity  = quantity;
	}
	return 0;
}

void print(Order order){ //TODO Borrar o hacer bien
	for(int i=0; i<numEntries; i++){
		order->items[i].product_id;
		order->items[i].quantity;
	}
}
