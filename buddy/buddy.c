
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "block.h"
#include "buddy.h"

/* memory pool size is 2^35 or 32GB */
#define MAX_MEM_SIZE 34359738368
#define DEFAULT_MEM_SIZE 536870912
static BlockPtr avail[35];
static char initFlag = 0;	//has buddy_init been called? 0:no, 1:yes

int main(int argc, char **argv)
{
	//test buddy_init
	printf("Before buddy_init.\n");
	buddy_init(DEFAULT_MEM_SIZE);
	printf("After buddy_init.\n");
	buddy_print();

	//test a single malloc
	buddy_malloc(32);
	buddy_print();


	exit(0);

}

void buddy_init(size_t initMemSize) {
	if(initMemSize > MAX_MEM_SIZE || initMemSize <= 0) {
		printf("Invalid memory size request. Greater than 32GB.");
		exit(1);
	}
	/*int i = 0;
	while(i < 35) {
		avail[i] == NULL;	//null out all slots in avail
	}*/
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
	head = create_block(1, NULL, NULL, size, pool);
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
	if(avail[power] != NULL) {
		mem = avail[power]->mem;
		remove_block(avail[power]);
	}
	else {
		count = power;
		while(avail[count] == NULL && count != 35) {
			count++;
		}
		printf("Count after first while:%d\n", count);
		if(count == 35) {
			//out of memory!!!!!
			printf("Out of memory!\n");
			errno = ENOMEM;
			exit(1);
		}
		//split the blocks
		while(count != power) {
			printf("Count:%d\n",count);
			split_block(avail[count]);
			count--;
		}
		printf("Count:%d , Power:%d\n", count, power);
		mem = avail[power]->mem;
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
	void *newMem;
	int size;

	//remove from avail
	remove_block(b);

	//setup new mem and change b's size
	newMem = b->mem + (1<<(b->header.kval / 2));
	b->header.kval = b->header.kval / 2;
	size = b->header.kval;
	newBlock = create_block(1, NULL, NULL, size, newMem);

	//reinsert into avail
	insert_block(b);
	insert_block(newBlock);
}

static void merge_blocks(BlockPtr b1, BlockPtr b2) {
	//remove both blocks from avail
	remove_block(b1);
	remove_block(b2);

	//setup b1 to be new combined block
	b1->header.kval += b2->header.kval;

	//put b1 back in avail
	insert_block(b1);
}

static void insert_block(BlockPtr block) {
	BlockPtr temp;
	//put block into avail
	if(avail[block->header.kval] != NULL) {
		temp = avail[block->header.kval];
		while(temp->header.linkf != NULL) {
			temp = temp->header.linkf;
		}
		temp->header.linkf = block;
		block->header.linkb = temp;
	}
	else {
		avail[block->header.kval] = block;
	}
}

static void remove_block(BlockPtr block) {
	if(block->header.linkf != NULL) {
		block->header.linkf->header.linkb = block->header.linkb;
	}
	if(block->header.linkb != NULL) {
		block->header.linkb->header.linkf = block->header.linkf;
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
		while(temp != NULL) {
			s = toString(temp);
			printf("%s\n", s);
			temp = temp->header.linkf;
		}
		i++;
	}
}
