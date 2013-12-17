#ifndef __BUDDY_H
#define __BUDDY_H

#include <unistd.h>
#include <stdlib.h>
#include "block.h"

void buddy_init(size_t);
void *buddy_calloc(size_t,size_t);
void *buddy_malloc(size_t);
void *buddy_realloc(void *,size_t);
void buddy_free(void *);
static void split_block(BlockPtr);
static void merge_blocks(BlockPtr, BlockPtr);
static void insert_block(BlockPtr);
static void remove_block(BlockPtr);
static int find_power(int);
