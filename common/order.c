#include "order.h"

struct order_t {
	orderEntry_t[] items;
	char * address;
	int cantEntries; 
};

struct order_entry_t{
	int product_id;
	int quantity;
};


//float getTotalPrice();
int getCantEntries(Order){ //Me confundi no se si sirve esto.
	return Order->cantEntries;
}
