
/*
	List.h: Defines the interface for a doubly-linked list.
*/

#ifndef __LIST_H
#define __LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "common.h"
#include "Node.h"

typedef struct list List;
typedef struct list * ListPtr;

struct list {
	int size;
	int maxSize;
	NodePtr head;
	NodePtr tail;
	unsigned long int (*getKey)(void *);
	char *  (*toString)(void *);
	void (*freeObject)(void *);
	pthread_mutex_t mutex;
	pthread_cond_t bufferNotEmpty, bufferNotFull;
};

/* prototypes of public methods */

/* constructor */
ListPtr createList(unsigned long int(*getKey)(void *), 
                   char * (*toString)(void *),
	           void (*freeObject)(void *),
                   int maxSize);
void freeList(ListPtr L);
void _freeList(ListPtr L); /* destructor */

int getSize(ListPtr L);
int getMaxSize(ListPtr L);

void finishUp(ListPtr L);

Boolean isEmpty(ListPtr L);

void addAtFront(ListPtr list, NodePtr node);
void _addAtFront(ListPtr list, NodePtr node);
void addAtRear(ListPtr list, NodePtr node);
void _addAtRear(ListPtr list, NodePtr node);

NodePtr removeFront(ListPtr list);
NodePtr _removeFront(ListPtr list);
NodePtr removeRear(ListPtr list);
NodePtr _removeRear(ListPtr list);
NodePtr removeNode(ListPtr list, NodePtr node);
NodePtr _removeNode(ListPtr list, NodePtr node);

NodePtr search(ListPtr list, unsigned long int);

void reverseList(ListPtr  L);
void _reverseList(ListPtr  L);

void printList(ListPtr L);


#endif /* __LIST_H */ 
