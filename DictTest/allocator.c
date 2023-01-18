#include "allocator.h"

allocator *core_allocator_init(int small_block_size)
{
	int alloc_block_size = core_alloc_block_align_size(sizeof(alloc_block));
	int allocator_size = core_alloc_block_align_size(sizeof(allocator));
	int min_size = alloc_block_size + allocator_size;
	allocator *alloc;
	if (min_size <= small_block_size) {
		alloc_block *small_block = core_alloc_block_init(small_block_size);
		alloc = core_alloc_block_alloc(small_block, allocator_size);
		alloc->small_first = small_block;
		alloc->small_last = small_block;
		alloc->num_small_blocks = 1;
		alloc->large_first = NULL;
		alloc->large_last = NULL;
		alloc->num_large_blocks = 0;
		alloc->allocated_on_large_block = 0;
	}
	else {
		alloc_block *large_block = core_alloc_block_init(min_size);
		alloc = core_alloc_block_alloc(large_block, allocator_size);
		alloc->small_first = NULL;
		alloc->small_last = NULL;
		alloc->num_small_blocks = 0;
		alloc->large_first = large_block;
		alloc->large_last = large_block;
		alloc->num_large_blocks = 1;
		alloc->allocated_on_large_block = 1;
	}
	alloc->small_block_size = small_block_size;
	return alloc;
}

void *core_allocator_alloc(allocator *alloc, int size)
{
	int aligned_size = core_alloc_block_align_size(size);
	if (aligned_size > alloc->small_block_size) {
		alloc_block *next = core_alloc_block_init(aligned_size + sizeof(alloc_block));
		if (alloc->large_last)
			alloc->large_last->next_block = next;
		alloc->large_last = next;
		alloc->num_large_blocks++;
		void *mem = core_alloc_block_alloc(alloc->large_last, aligned_size);
		return mem;
	}

	void *mem = core_alloc_block_alloc(alloc->small_last, aligned_size);
	if (mem == NULL) {
		alloc_block *next = core_alloc_block_init(alloc->small_block_size);
		if (alloc->small_last)
			alloc->small_last->next_block = next;
		alloc->small_last = next;
		alloc->num_small_blocks++;
		mem = core_alloc_block_alloc(alloc->small_last, aligned_size);
	}

	return mem;
}

void core_allocator_free(allocator *alloc)
{
	alloc_block *first_blocks[2];
	if (alloc->allocated_on_large_block) {
		first_blocks[0] = alloc->small_first;
		first_blocks[1] = alloc->large_first;
	}
	else {
		first_blocks[0] = alloc->large_first;
		first_blocks[1] = alloc->small_first;
	}

	for (int i = 0; i < 2; i++) {
		alloc_block *block = first_blocks[i];
		while (block) {
			alloc_block *next = block->next_block;
			core_alloc_block_free(block);
			block = next;
		}
	}
}