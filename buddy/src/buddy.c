
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "block.h"
#incluce "buddy.h"

/* memory pool size is 2^35 or 32GB */
#define MAX_MEM_SIZE 34359738368
#define DEFAULT_MEM_SIZE 536870912
static BlockPtr avail[35];
static char initFlag = 0;	//has buddy_init been called? 0:no, 1:yes

int main(int argc, char **argv)
{
	int i;
	int value;
	int *slot;

	/*void *pool = sbrk(1LL << 48);*/
	void *pool = sbrk(1 << 22);
	if (pool < 0 || errno == ENOMEM) {
		perror("Could not allocate memory pool!");
		exit(1);
	}
	/* Now pool is an array of MAX_MEM_SIZE  bytes */

	printf("starting address for memory pool  = %p\n", pool);
	for (i=0; i<1000000; i++) {
		slot = pool+sizeof(int)*i;
		*slot = i;
	}

	for (i=0; i<10; i++) {
		memcpy(&value, pool+sizeof(int)*i, sizeof(int)); 
		printf("int stored at slot[%d]=%d\n", i, value);
		/*printf("int stored at slot[%d]=%d\n", i, *(int *)(pool+sizeof(int)*i));*/
		/*int * array = (int *) pool; array[i]*/
	}
	/*int *x = (int *) malloc(sizeof(int));*/
	void *pool2 = sbrk(1 << 22);
	if (pool2 < 0 || errno == ENOMEM) {
		perror("Could not allocate memory pool!");
		exit(1);
	}
	printf("starting address for memory pool  = %p\n", pool2);
	for (i=0; i<1000000; i++) {
		slot = pool2+sizeof(int)*i;
		*slot = i;
	}
	for (i=0; i<10; i++) {
		memcpy(&value, pool2+sizeof(int)*i, sizeof(int)); 
		printf("int stored at slot[%d]=%d\n", i, value);
	}

	exit(0);

}

void buddy_init(size_t initMemSize) {
	if(initMemSize > MAX_MEM_SIZE) {
		printf('Invalid memory size request. Greater than 32GB.');
		exit(1);
	}
	initFlag = 1;
	BlockPtr head;
	int size;
	void *pool = sbrk(initMemSize);
	if (pool < 0 || errno == ENOMEM) {
		perror("Could not allocate memory pool!");
		exit(1);
	}
	/* Now pool is an array of initMemSize  bytes */
	size = find_power(initMemSize);
	head = create_block(1, null, null, size, pool);
	avail[size] = head;
}

void *buddy_calloc(size_t,size_t) {
	if(initFlag != 1) {
		buddy_init(DEFAULT_MEM_SIZE);
	}
}

void *buddy_malloc(size_t size) {
	if(initFlag != 1) {
		buddy_init(DEFAULT_MEM_SIZE);
	}
	int pow = find_power((int)size);
	if(avail[pow] != null) {

	}
}

void *buddy_realloc(void *,size_t) {
	if(initFlag != 1) {
		buddy_init(DEFAULT_MEM_SIZE);
	}
}

void buddy_free(void *) {

}

static void *split_block(BlockPtr b) {
	void *newBlock;
	BlockPtr temp;

	//remove from avail
	if(b->header->linkf != null) {
		b->header->linkf->header->linkb = b->header->linkb;
	}
	if(b->header->linkb != null) {
		b->header->linkb->header->linkf = b->header->linkf;
	}

	//setup new mem and change b's size
	newBlock = b->mem + (1<<(b->header->kval / 2));
	b->header->kval = b->header->kval / 2;

	//reinsert into avail
	if(avail[b->header->kval] != null) {
		temp = avail[b->header->kval];
		while(temp->header->linkf != null) {
			temp = temp->header->linkf;
		}
		temp->header->linkf = b;
		b->header->linkb = temp;
	}
	else {
		avail[b->header->kval] = b;
	}
	return newBlock;
}

static void merge_blocks(BlockPtr b1, BlockPtr b2) {

}

static int find_power(int n) {
	int count = 0;
	//n is a power of 2
	if (n && !(n&(n-1))) {
		while(n != 1) {
			n  >>= 1;
			count += 1;
		}
		return count;
	}

	//n is not a power of 2
	while(n != 0) {
		n  >>= 1;
		count += 1;
	}
	return count;
}
