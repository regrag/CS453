
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "block.h"
#include "buddy.h"

/* max memory pool size is 2^35 or 32GB */
#define MAX_MEM_SIZE 34359738368
#define DEFAULT_MEM_SIZE 536870912 //512MB

HeaderPtr avail[35];
static char initFlag = 0;	//has buddy_init been called? 0:no, 1:yes
static size_t baseAddr;		//base address of the pool

/*
* Parameters: argc, argv - not used
* Return: int - 0 if exitted normally, 1 if not
* This function is what is compiled as the main of the buddy system. It is used for unit
* level testing and nothing else.
*/
int main(int argc, char **argv)
{
	void *temp;
	void *temp2;
	void *buddyFound;
	//test buddy_init
	printf("buddy_init 1024 Test.\n");
	buddy_init(1024);
	buddy_print();

	//test a single malloc
	printf("buddy single malloc 32 test.\n");
	temp = buddy_malloc(32);
	buddy_print();

	printf("find_buddy test.\n");
	buddyFound = find_buddy(temp);
	printf("buddyFound: %p\n", buddyFound);

	printf("buddy single malloc 32 test 2.\n");
	temp2 = buddy_malloc(32);
	buddy_print();

	printf("buddy free test, freeing two blocks.\n");
	buddy_free(temp);
	buddy_free(temp2);
	buddy_print();

	exit(0);

}

/*
* Parameters: size_t initMemSize - used for initial memory pool size
* Returns: void
* This function is used to setup the memory pool and pointers that will
* be used in the buddy system.
*/
void buddy_init(size_t initMemSize) {
	if(initMemSize > MAX_MEM_SIZE || initMemSize <= 0) {
		printf("Invalid memory size request. Greater than 32GB.");
		exit(1);
	}
	initFlag = 1;
	BlockPtr block;
	int size;

	int i = 0;
	HeaderPtr temp;
	while(i<35) {
		temp = sbrk(sizeof(Header));
		avail[i] = temp;
		i++;
	}

	void *pool = sbrk(initMemSize);
	baseAddr = pool;
	if (pool < 0 || errno == ENOMEM) {
		perror("Could not allocate memory pool!");
		exit(1);
	}
	/* Now pool is an array of initMemSize  bytes */
	size = find_power(initMemSize);
	block = create_block(1, NULL, NULL, size, pool);
	insert_front(block);
}

/*
* Parameters: size_t size - size of data type, size_t num - number of data types to alloc
* Returns: void* - pointer to alloc'ed memory
* This function is used to alloc a number of data type of size.
*/
void *buddy_calloc(size_t size, size_t num) {
	void *mem = buddy_malloc(num * size);
	memset(mem, 0, (size*num));
	return mem;
}

/*
* Parameters: size_t size - size of memory requested
* Returns: void* - pointer to alloc'ed memory
* This function is used to alloc a memory block of size.
*/
void *buddy_malloc(size_t size) {
	void *mem;
	int count;
	if(initFlag != 1) {
		buddy_init(DEFAULT_MEM_SIZE);
	}

	if(size < 32)
		size = 32;	//so the mem block is big enough to store its own header

	int power = find_power((int)size);
	if(avail[power]->head != NULL) {
		mem = avail[power]->head;
		remove_front(power);
	}
	else {
		count = power;
		while(avail[count]->head == NULL && count != 35) {
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
			split_block(avail[count]->head);
			count--;
		}
		mem = avail[power]->head;
		remove_front(power);
	}

	return mem;
}

/*
* Parameters: void* pointer - pointer from user that needs to be re-alloc'ed to,
* size_t size - the desired size of the pointer returned
* Returns: void* - pointer to alloc'ed memory
* This function is used to realloc an existting pointer to be a different size.
*/
void *buddy_realloc(void * pointer, size_t size) {
	if(initFlag != 1) {
		buddy_init(DEFAULT_MEM_SIZE);
	}
	void *mem;
	buddy_free(pointer);
	mem = buddy_malloc(size);
	return mem;
}

/*
* Parameters: void* pointer - pointer to memory to be freed.
* Returns: void
* This function will "free" a pointer and put it back into the pool.
*/
void buddy_free(void *pointer) {
	BlockPtr newBlock;
	int power = find_power(sizeof(pointer));
	BlockPtr freedBlock = create_block(1, NULL, NULL, power, pointer);
	BlockPtr buddyFound = find_buddy(freedBlock);
	if(buddyFound != NULL) {
		merge_blocks(freedBlock, buddyFound);
	}
	else {
		insert_front(freedBlock);
	}
}

/*
* Parameters: BlockPtr b - pointer to block to split
* Returns: void
* This function will split a block into two new blocks and insert them
* both into the pool.
*/
static void split_block(BlockPtr b) {
	printf("Entered split_block, splitting:%p\n", b);
	BlockPtr newBlock;
	BlockPtr temp = b;
	int size;

	//remove from avail
	remove_front(b->kval);

	//setup new mem and change b's size
	b->kval -= 1;
	printf("b kval editted.\n");
	newBlock = create_block(1, NULL, NULL, b->kval, (void*)(b + (1<<b->kval)));
	printf("Block created inside split block.\n");
	//reinsert into avail
	insert_front(newBlock);
	insert_front(b);
}

/*
* Parameters: BlockPtr block - pointer to block to find the buddy of
* Returns: BlockPtr - points to found buddy or null if not found
* This function finds the buddy of a passed in block.
*/
static BlockPtr find_buddy(BlockPtr block) {
	BlockPtr buddy = (BlockPtr)((((size_t)block-baseAddr)^(1<<block->kval)) + baseAddr);
	return buddy;


	/*BlockPtr search = avail[block->kval]->head;
	void *possibleBuddy1 = block+(1<<block->kval);
	void *possibleBuddy2 = block-(1<<block->kval);
	//DEBUG printf("block:%p | looking:%p,%p\n",block, possibleBuddy1,possibleBuddy2);
	while(search != NULL) {
		if(possibleBuddy1 == search)
			return possibleBuddy1;
		if(possibleBuddy2 == search)
			return possibleBuddy2;
		search = search->linkf;
	}
	return NULL;*/
}

/*
* Parameters: BlockPtr b1 - pointer to first block, BlockPtr b2 - pointer to second block
* Returns: void
* This function will merge the two passed in blocks into one and insert it into the pool
*/
static void merge_blocks(BlockPtr b1, BlockPtr b2) {
	//remove both blocks from avail
	remove_front(b1->kval);

	//setup b1 to be new combined block
	if(b1 > b2) {
		b2->kval += 1;
		insert_front(b2);
	}
	else {
		b1->kval += 1;
		insert_front(b1);
	}
}

/*
* Parameters: BlockPtr block - pointer to block to insert into front of a list
* Returns: void
* This function will insert the passed in block into the front of a list in avail.
*/
static void insert_front(BlockPtr block) {
	//put block into avail
	printf("Entered insert_front, inserting:%p\n", block);
	if(avail[block->kval]->head != NULL) {
		avail[block->kval]->head->linkb = block;
		block->linkf = avail[block->kval]->head;
		block->linkb = NULL;
		avail[block->kval]->head =  block;
	}
	else {
		avail[block->kval]->head = block;
	}
	//DEBUG printf("Exitting insert_front, insertted:%p\n", block);
}

/*
* Parameters: int power - used to find which list to pull from
* Returns: void
* This function will remove a block from the front of a list specified by power.
*/
static void remove_front(int power) {
	BlockPtr temp;
	printf("Entered remove_front, removing at:%d\n", power);
	if(avail[power]->head->linkf != NULL)
		avail[power]->head->linkf->linkb = NULL;
	if(avail[power]->head->linkb != NULL)
		avail[power]->head->linkb->linkf = NULL;
	temp = avail[power]->head;
	avail[power]->head = avail[power]->head->linkf;
	temp->linkf = NULL;
	temp->linkb = NULL;
	temp->tag = 0;
	printf("Exitting remove_front, removing at:%d\n", power);
}

/*
* Parameters: int n - number to find the nearest power of two of
* Returns: int
* This function will find the nearest power of two to the passed in value.
*/
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

/*
* Parameters: void
* Returns: void
* This function will print out the buddy system list.
*/
static void buddy_print() {
	BlockPtr temp;
	char* s;
	int i = 0;
	while(i < 35) {
		temp = avail[i]->head;
		printf("avail[%d]:\n",i);
		while(temp != NULL) {
			s = toString(temp);
			printf("  %s\n", s);
			temp = temp->linkf;
		}
		i++;
	}
}
