#ifndef __BLOCK_H
#define __BLOCK_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct block {
	struct {
		char tag;
		struct block *linkf;
		struct block *linkb;
		unsigned int kval;
	} header;
	void *mem;
};

typedef struct block Block;
typedef struct block * BlockPtr;

BlockPtr create_block(int, BlockPtr, BlockPtr, unsigned int, void *sysmem);
char *toString(BlockPtr);

#endif