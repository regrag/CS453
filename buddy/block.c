#include "block.h"

BlockPtr create_block(int newTag, BlockPtr next, 
						BlockPtr prev, unsigned int size, void *blockMem) {
	BlockPtr block = (BlockPtr) blockMem;
	block->tag = newTag;
	block->linkf = next;
	block->linkb = prev;
	block->kval = size;
	return block;
}

char *toString(BlockPtr block) {
	char string[125];
	sprintf(string, "[tag=%d,kval=%d,addr=%p]", block->tag, block->kval,
												  block);
	return string;
}
