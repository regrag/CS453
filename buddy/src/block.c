#include "block.h"

HeaderPtr create_block(char newTag, BlockPtr next, 
						BlockPtr prev, unsigned int size, void *sysmem) {
	BlockPtr block;
	block->header->tag = newTag;
	block->header->linkf = next;
	block->header->linkb = prev;
	block->header->kval = size;
	block->mem = sysmem;
	return block;
}

char *toString(BlockPtr block) {
	char string[30];
	sprintf(string, "[tag=%c,kval=%d,addr=%p]\n", block->header->tag, block->header->kval,
												  block->mem);
	return string;
}
