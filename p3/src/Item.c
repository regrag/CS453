
#include "Item.h"

ItemPtr createItem(int index, int producer) 
{
	ItemPtr item = (ItemPtr) malloc(sizeof(Item));
	item->id = index; // this gets filled in by the pool
	item->producer = producer;
	return item;
}

void setItemKey(ItemPtr item, int id) 
{ 
	item->id = id; 
}


unsigned long int getItemKey(void *obj) 
{ 
	return ((ItemPtr)obj)->id;
}

#define ITEM_STRING_SIZE 128

char *toItemString(void *obj)
{
	ItemPtr item = (ItemPtr) obj;
	char *buffer = (char *) malloc(sizeof(char)*ITEM_STRING_SIZE);
	sprintf(buffer, "%d from producer %d ", item->id, item->producer);
	return buffer;
}

void freeItem(void *obj) 
{
	free((ItemPtr) obj);
}
