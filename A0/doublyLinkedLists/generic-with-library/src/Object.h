

#ifndef __OBJECT_H
#define __OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include "common.h"

#define MAXPID_DIGITS 20


typedef struct object  Object;
typedef struct object * ObjectPtr;

struct object {
	unsigned long int key;
	char *data;
};

ObjectPtr createObject (unsigned long int, char *);

char *toString(void *);
void freeObject(void *);
unsigned long int getKey(void *);
int compareObjects(void *, void *);


#endif /* __OBJECT_H */
