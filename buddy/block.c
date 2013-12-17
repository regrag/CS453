#include "block.h"

BlockPtr create_block(int newTag, BlockPtr next, 
						BlockPtr prev, unsigned int size, void *sysmem) {
	static Block b;
	BlockPtr block = &b;
	block->header.tag = newTag;
	block->header.linkf = next;
	block->header.linkb = prev;
	block->header.kval = size;
	block->mem = sysmem;
	return block;
}

char *toString(BlockPtr block) {
	char string[125];
	sprintf(string, "[tag=%d,kval=%d,addr=%p]", block->header.tag, block->header.kval,
												  block->mem);
	return string;
}
