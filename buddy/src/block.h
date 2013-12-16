#ifndef __BLOCK_H
#define __BLOCK_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct block {
	struct {
		char tag;
		struct header *linkf;
		struct header *linkb;
		unsigned int kval;
	} header;
	void *mem;
};

typedef struct block Block;
typedef struct block * BlockPtr;

BlockPtr create_block(char, BlockPtr, BlockPtr, unsigned int, void *sysmem);
void free_block(BlockPtr);
char *toString(BlockPtr);
