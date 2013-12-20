#include "block.h"

/*
* Parameters: newTag - used to denote whether or not the block is reserved,
* BlockPtr next - next block in list this block is in, BlockPtr prev - prev
* block in list this block is in, unsigned int size - kval for the block,
* void* blockMem - starting memory address for the new block.
* Returns: BlockPtr
* This function will create a new block.
*/
BlockPtr create_block(int newTag, BlockPtr next, 
						BlockPtr prev, unsigned int size, void *blockMem) {
	BlockPtr block = (BlockPtr) blockMem;
	block->tag = newTag;
	block->linkf = next;
	block->linkb = prev;
	block->kval = size;
	return block;
}

/*
* Parameters: BlockPtr block - pointer to the block to be stringafied
* Returns: void
* This function will return a string representation of a block.
*/
char *toString(BlockPtr block) {
	char string[125];
	sprintf(string, "[tag=%d,kval=%d,addr=%p]", block->tag, block->kval,
												  block);
	return string;
}
