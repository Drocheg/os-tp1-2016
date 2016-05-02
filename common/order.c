#include "order.h"

struct orderEntry_t{
	int product_id;
	int quantity;
};

struct order_t {
	struct orderEntry_t items[16]; //Se podria hacer dinamico
	char * address;
	int numEntries; 
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

void printTemporalOrder(Order order){ //TODO Borrar o hacer bien
	for(int i=0; i<order->numEntries; i++){
		printf("%d\n", order->items[i].product_id);
		printf("%d\n", order->items[i].quantity);
	}
}
