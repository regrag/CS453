
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "block.h"
#include "buddy.h"

/* max memory pool size is 2^35 or 32GB */
#define MAX_MEM_SIZE 34359738368
#define DEFAULT_MEM_SIZE 536870912 //512MB

static BlockPtr avail[35];
static char initFlag = 0;	//has buddy_init been called? 0:no, 1:yes

int main(int argc, char **argv)
{
	//test buddy_init
	printf("buddy_init Test.\n");
	buddy_init(64);
	buddy_print();

	//test a single malloc
	printf("buddy single malloc test.\n");
	buddy_malloc(32);
	buddy_print();


	exit(0);

}

void buddy_init(size_t initMemSize) {
	if(initMemSize > MAX_MEM_SIZE || initMemSize <= 0) {
		printf("Invalid memory size request. Greater than 32GB.");
		exit(1);
	}
	initFlag = 1;
	BlockPtr block;
	int size;
	void *pool = sbrk(initMemSize);
	if (pool < 0 || errno == ENOMEM) {
		perror("Could not allocate memory pool!");
		exit(1);
	}
	/* Now pool is an array of initMemSize  bytes */
	size = find_power(initMemSize);
	block = create_block(1, NULL, NULL, size, pool);
	avail[size] = block;
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

	if(size < 32)
		size = 32;	//so the mem block is big enough to store its own header

	int power = find_power((int)size);
	if(avail[power] != NULL) {
		mem = avail[power];
		remove_block(avail[power]);
	}
	else {
		count = power;
		while(avail[count] == NULL && count != 35) {
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
		mem = avail[power];
		remove_block(avail[power]);
	}

	return mem;
}

void *buddy_realloc(void * pointer, size_t size) {
	if(initFlag != 1) {
		buddy_init(DEFAULT_MEM_SIZE);
	}
	void *mem;

	return mem;
}

void buddy_free(void *pointer) {

}

static void split_block(BlockPtr b) {
	BlockPtr newBlock;
	int size;

	//remove from avail
	remove_block(b);

	//setup new mem and change b's size
	b->kval = b->kval - 1;
	newBlock = create_block(1, NULL, NULL, b->kval, b + b->kval);

	//reinsert into avail
	insert_block(b);
	insert_block(newBlock);
}

static void merge_blocks(BlockPtr b1, BlockPtr b2) {
	//remove both blocks from avail
	remove_block(b1);
	remove_block(b2);

	//setup b1 to be new combined block
	b1->kval += b2->kval;

	//put b1 back in avail
	insert_block(b1);
}

static void insert_block(BlockPtr block) {
	//put block into avail
	if(avail[block->kval] != NULL) {
		avail[block->kval]->linkb = block;
		block->linkf = avail[block->kval];
		avail[block->kval] =  block;
		/*printf("avail[block->kval] = %p\n", avail[block->kval]);
		printf("avail[block->kval]->linkf = %p\n", avail[block->kval]->linkf);
		printf("avail[block->kval]->linkb = %p\n", avail[block->kval]->linkb);
		printf("avail[block->kval]->linkf->linkb = %p\n", avail[block->kval]->linkf->linkb);
		printf("avail[block->kval]->linkf->linkf = %p\n", avail[block->kval]->linkf->linkf);*/
	}
	else {
		avail[block->kval] = block;
	}
}

static void remove_block(BlockPtr block) {
	if(block->linkf != NULL) {
		block->linkf->linkb = block->linkb;
	}
	if(block->linkb != NULL) {
		block->linkb->linkf = block->linkf;
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

static void buddy_print() {
	BlockPtr temp;
	char* s;
	int i = 0;
	while(i < 35) {
		temp = avail[i];
		printf("avail[%d]:%p\n",i,avail[i]);
		while(temp != NULL) {
			s = toString(temp);
			printf("%s\n", s);
			temp = temp->linkf;
		}
		i++;
	}
}
