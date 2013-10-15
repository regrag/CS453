#include "Object.h"


ObjectPtr createObject(unsigned long int key, char *data)
{
	ObjectPtr newObject = (ObjectPtr) malloc (sizeof(Object));
	newObject->key = key;
	newObject->data = (char *) malloc(sizeof(char)*(strlen(data)+1));
	strcpy(newObject->data, data); 
	return newObject;
}

unsigned long int getKey(void *obj)
{
	ObjectPtr myobj = (ObjectPtr) obj;
	return myobj->key;
}

char *toString(void *obj)
{
	ObjectPtr myobj = (ObjectPtr) obj;
	char *temp;
	temp = (char *)malloc(sizeof(char)*strlen(myobj->data)+1+MAXPID_DIGITS+4);
	sprintf(temp, "[%ld] %s", myobj->key, myobj->data);
	return temp;
}

void freeObject(void *obj)
{
	ObjectPtr myobj = (ObjectPtr) obj;
	if (myobj == NULL) return;
	free(myobj->data);
	free(myobj);
}

int compareObjects(void *a, void *b) {
	if (((ObjectPtr)a)->key == ((ObjectPtr)b)->key) return 1;
	else return 0;
}
