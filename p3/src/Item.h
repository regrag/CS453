
#ifndef __ITEM_H
#define __ITEM_H

#include <malloc.h>
#include <stdio.h>

typedef struct item Item;
typedef struct item * ItemPtr;

struct item {
	int id;
	int producer;
};

ItemPtr createItem(int index, int producer);
void setItemKey(ItemPtr item, int id);
unsigned long int getItemKey(void *);
char *toItemString(void *);
void freeItem(void *);



#endif /* __ITEM_H */
