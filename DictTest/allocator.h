#ifndef CORE_ALLOCATOR_H
#define CORE_ALLOCATOR_H

#include "allocation_block.h"

typedef struct {
	int small_block_size;
	int allocated_on_large_block;
	alloc_block *small_first;
	alloc_block *small_last;
	int num_small_blocks;
	alloc_block *large_first;
	alloc_block *large_last;
	int num_large_blocks;
} allocator;

allocator *core_allocator_init(int small_block_size);

void *core_allocator_alloc(allocator *alloc, int size);

void core_allocator_free(allocator *alloc);

#endif // CORE_ALLOCATOR_H
