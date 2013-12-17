
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

void *buddy_calloc(size_t size, size_t num) {
	void *mem = buddy_malloc(num * size);
	memset(mem, 0, (size*num));
	return mem;
}

void *buddy_malloc(size_t size) {
	void *mem;
	int count;
	if(initFlag != 1) {
		buddy_init(DEFAULT_MEM_SIZE);
	}

	int power = find_power((int)size);
	if(avail[power] != null) {
		mem = avail[power]->mem;
		remove_block(avail[power]);
	}
	else {
		count = power;
		while(avail[count] == null && count != 35) {
			count++;
		}
		if(count == 35) {
			//out of memory!!!!!
			printf("Out of memory!\n");
			errno = ENOMEM;
			exit(1);
		}
		//split the blocks
		while(count != power) {
			split_block(avail[count]);
			count--;
		}
		mem = avail[power]->mem;
		remove_block(avail[power]);
	}

	return mem;
}

void *buddy_realloc(void * pointer, size_t size) {
	if(initFlag != 1) {
		buddy_init(DEFAULT_MEM_SIZE);
	}
}

void buddy_free(void *) {

}

static void *split_block(BlockPtr b) {
	BlockPtr newBlock;

	//remove from avail
	remove_block(b);

	//setup new mem and change b's size
	newBlock->mem = b->mem + (1<<(b->header->kval / 2));
	b->header->kval = b->header->kval / 2;
	newBlock->header->kval = b->header->kval;

	//reinsert into avail
	insert_block(b);
	insert_block(newBlock);
}

static void merge_blocks(BlockPtr b1, BlockPtr b2) {
	//remove both blocks from avail
	remove_block(b1);
	remove_block(b2);

	//setup b1 to be new combined block
	b1->header->kval += b2->header->kval;

	//put b1 back in avail
	insert_block(b1);
}

static void insert_block(BlockPtr block) {
	BlockPtr temp;
	//put block into avail
	if(avail[block->header->kval] != null) {
		temp = avail[block->header->kval];
		while(temp->header->linkf != null) {
			temp = temp->header->linkf;
		}
		temp->header->linkf = block;
		block->header->linkb = temp;
	}
	else {
		avail[block->header->kval] = block;
	}
}

static void remove_block(BlockPtr block) {
	if(block->header->linkf != null) {
		block->header->linkf->header->linkb = block->header->linkb;
	}
	if(block->header->linkb != null) {
		block->header->linkb->header->linkf = block->header->linkf;
	}
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
