
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "List.h"

/*

	list.c 
		Contains functions to manipulate a doubly-linked list.
 
*/


/* private methods */

static NodePtr reverse(NodePtr);
static void print(NodePtr node, char * (*toString)(void *));

ListPtr createList(unsigned long int (*getKey)(void *), 
                   char * (*toString)(void *),
	           void (*freeObject)(void *),
                   int maxSize)
{
	ListPtr list;
	list = (ListPtr) malloc(sizeof(List));
	list->size = 0;
	list->maxSize = maxSize;
	list->head = NULL;
	list->tail = NULL;
	list->getKey = getKey;
	list->toString = toString;
	list->freeObject = freeObject;
	pthread_cond_init(&(list->bufferNotEmpty), NULL);
	pthread_cond_init(&(list->bufferNotFull), NULL);
	pthread__mutex_init(&(list->mutex), NULL);
    pthread_cond_broadcast(&(list->bufferNotEmpty));
    pthread_cond_broadcast(&(list->bufferNotFull));
	return list;
}

/*
* Parameters: list pointer
* Returns: void
* This function will free the memory allocated for the list.
*/
void freeList(ListPtr list) {
	pthread__mutex_lock(&(list->mutex));
	_freeList(list);
	pthread__mutex_unlock(&(list->mutex));
}

void _freeList(ListPtr list)
{
	if(list == NULL) return;
	while(list->size > 0) {
		freeNode(removeFront(list), list->freeObject);
	}
	free(list);
}

/*
* Paramters: list pointer
* Returns: The size of the passed list if not null.
*/
int getSize(ListPtr list)
{
	if(list == NULL) return NULL;
	return list->size;
}

int getMaxSize(ListPtr list) {
	if(list == NULL) return NULL;
	return list->maxSize;
}

void finishUp(ListPtr list) {
	pthread__mutex_lock(&(list->mutex));
	pthread_cond_broadcast(&(list->bufferNotEmpty));
	pthread_cond_broadcast(&(list->bufferNotFull));
	pthread__mutex_unlock(&(list->mutex));
}

/*
* Parameters: list pointer
* Return: True if list is emptry, else false
* This function will check to see if the passed list is empty based off of its size param.
*/
Boolean isEmpty(ListPtr list)
{
	if (list->size == 0)
		return TRUE;
	else
		return FALSE;
}

/*
* Parameters: list pointer, node pointer
* Returns: void
* This function adds the passed node to the front of the passed list and updates
* all pointers.
*/
void addAtFront(ListPtr list, NodePtr node) {
	pthread__mutex_lock(&(list->mutex));
	while(getSize(list) == getMaxSize(list)) {
		//list is full
		pthread_cond_wait(&(list->bufferNotFull), &(list->mutex));
	}
	_addAtFront(list, node);
	pthread__mutex_unlock(&(list->mutex));
	pthread_cond_broadcast(&(list->bufferNotEmpty));
}

void _addAtFront(ListPtr list, NodePtr node)
{
	if (list == NULL) return;
	if (node == NULL) return;
	list->size++;
	node->next = list->head;
	node->prev = NULL;
	if (list->head == NULL)
	{
		list->head = node;
		list->tail = node;
	} else {
		list->head->prev = node;
		list->head = node;
	}
}

/*
* Parameters: list pointer, node pointer
* Returns: void
* This function adds the passed node to the rear of the passed list and updates
* all pointers.
*/
void addAtRear(ListPtr list, NodePtr node) {
	pthread__mutex_lock(&(list->mutex));
	while(getSize(list) == getMaxSize(list)) {
		//list is full
		pthread_cond_wait(&(list->bufferNotFull), &(list->mutex));
	}
	_addAtRear(list, node);
	pthread__mutex_unlock(&(list->mutex));
	pthread_cond_broadcast(&(list->bufferNotEmpty));
}

void _addAtRear(ListPtr list, NodePtr node)
{
	if (list == NULL) return;
	if (node == NULL) return;
	list->size++;
	node->next = NULL;
	node->prev = list->tail;
	if (list->head == NULL)
	{
		list->head = node;
		list->tail = node;
	} else {
		list->tail->next = node;
		list->tail = node;
	}
}

/*
* Paramters: list pointer
* Returns: node pointer
* This function will remove the head node of the list if it exists and update all pointers.
*/
NodePtr removeFront(ListPtr list) {
	NodePtr nodePtr;
	pthread__mutex_lock(&(list->mutex));
	while(getSize(list) == 0) {
		//list is empty
		pthread_cond_wait(&(list->bufferNotEmpty), &(list->mutex));
	}
	nodePtr = _removeFront(list);
	pthread__mutex_unlock(&(list->mutex));
	pthread_cond_broadcast(&(list->bufferNotFull));
	return nodePtr;
}

NodePtr _removeFront(ListPtr list)
{
	NodePtr nodePtr;
	if (list == NULL) return NULL;
	if (list->size == 0) return NULL;
	//corner case
	if (list->size == 1) {
		list->size--;
		nodePtr = list->head;
		list->head = list->head->next;
		return nodePtr;
	}
	else {
		list->size--;
		list->head->next->prev = NULL;
		nodePtr = list->head;
		list->head = list->head->next;
		return nodePtr;
	}
}

/*
* Paramters: list pointer
* Returns: node pointer
* This function will remove the tail node of the list if it exists and update all pointers.
*/
NodePtr removeRear(ListPtr list) {
	NodePtr nodePtr;
	pthread__mutex_lock(&(list->mutex));
	while(getSize(list) == 0) {
		//list is empty
		pthread_cond_wait(&(list->bufferNotEmpty), &(list->mutex));
	}
	nodePtr = _removeRear(list);
	pthread__mutex_unlock(&(list->mutex));
	pthread_cond_broadcast(&(list->bufferNotFull));
	return nodePtr;
}

NodePtr _removeRear(ListPtr list)
{
	NodePtr nodePtr;
	if (list == NULL) return NULL;
	if (list->size == 0) return NULL;
	//corner case
	if (list->size == 1) {
		list->size--;
		nodePtr = list->head;
		list->head = list->head->next;
		return nodePtr;
	}
	else {
		list->size--;
		list->tail->prev->next = NULL;
		nodePtr = list->tail;
		list->tail = list->tail->prev;
		return nodePtr;
	}
}

/*
* Paramters: list pointer, node pointer, function pointer
* Returns: node pointer if node is found, else NULL
* This function will search the passed list for the passed node by using the list
* comparison method for the data of the nodes. The node is removed if found.
*/
NodePtr removeNode(ListPtr list, NodePtr node) {
	NodePtr nodePtr;
	pthread__mutex_lock(&(list->mutex));
	while(getSize(list) == 0) {
		//list is empty
		pthread_cond_wait(&(list->bufferNotEmpty), &(list->mutex));
	}
	nodePtr = _removeNode(list, node);
	pthread__mutex_unlock(&(list->mutex));
	pthread_cond_broadcast(&(list->bufferNotFull));
	return nodePtr;
}

NodePtr _removeNode(ListPtr list, NodePtr node)
{
	int i;
	NodePtr searchPtr;
	if (list == NULL) return NULL;
	if (node == NULL) return NULL;
	if (list->size == 0) return NULL;
	i = 1;
	//Two corner cases
	if(list->getKey(list->head) == list->getKey(node)) return removeFront(list);
	if(list->getKey(list->tail) == list->getKey(node)) return removeRear(list);
	searchPtr = list->head->next;
	while (i < list->size) {
		if(list->getKey(searchPtr->obj) == list->getKey(node->obj)) {
			searchPtr->next->prev = searchPtr->prev;
			searchPtr->prev->next = searchPtr->next;
			list->size--;
			return searchPtr;
		}
		searchPtr = searchPtr->next;
		i ++;
	}
	//Node did not exist, return NULL
	return NULL;
}

/*
* Paramters: list pointer, node pointer, function pointer
* Returns: node pointer if node is found, else NULL
* This function will search the passed list for the passed node by using the list
* getkey comparison method for the data of the nodes.
*/
NodePtr search(ListPtr list, unsigned long int key)
{
	int i;
	NodePtr searchPtr;
	if (list == NULL) return NULL;
	if (key == NULL ) return NULL;
	if (list->size == 0) return NULL;
	i = 0;
	searchPtr = list->head;
	while (i < list->size) {
		if (list->getKey(searchPtr->obj) == key) {
			return searchPtr;
		}
		searchPtr = searchPtr->next;
		i ++;
	}
	//Node not found.
	return NULL;
}

void reverseList(ListPtr list) {
	pthread__mutex_lock(&(list->mutex));
	reverseList(list);
	pthread__mutex_unlock(&(list->mutex));
}

void _reverseList(ListPtr list)
{
	list->tail = list->head;
	list->head  = reverse (list->head);
}

static NodePtr reverse(NodePtr L)
{
	NodePtr list = NULL;
	while (L != NULL) {
		NodePtr tmp = L;
		L = L->next;
		if (L != NULL) L->prev = tmp;
		tmp->next = list;
		tmp->prev = L;
		list = tmp;
	}
	return list;
}

void printList(ListPtr list)
{
	if (list) print(list->head, list->toString);
}

static void print(NodePtr node, char * (*toString)(void *))
{
	int count = 0;
	char *output;

	while (node) {
		output = (*toString)(node->obj);
		printf(" %s -->",output);
		free(output);
		node = node->next;
		count++;
		if ((count % 6) == 0)
			printf("\n");
	}
    printf(" NULL \n");
}
 

		

