#include "allocator.h"

allocator *core_allocator_init(int size)
{
	int alloc_block_size = core_alloc_block_align_size(sizeof(alloc_block));
	int allocator_size = core_alloc_block_align_size(sizeof(allocator));
	if (size < alloc_block_size + allocator_size)
		return NULL;
	alloc_block *first = core_alloc_block_init(size);
	allocator *alloc = core_alloc_block_alloc(first, allocator_size);
	alloc->block_size = size;
	alloc->first = first;
	alloc->last = first;
	alloc->num_blocks = 1;
	return alloc;
}

void *core_allocator_alloc(allocator *alloc, int size)
{
	int aligned_size = core_alloc_block_align_size(size);
	if (aligned_size > alloc->block_size)
		return NULL;

	void *mem = core_alloc_block_alloc(alloc->last, size);
	if (mem == NULL) {
		alloc_block *next = core_alloc_block_init(alloc->block_size);
		alloc->last->next_block = next;
		alloc->last = next;
		alloc->num_blocks++;
		mem = core_alloc_block_alloc(alloc->last, size);
	}

	return mem;
}

void core_allocator_free(allocator *alloc)
{
	alloc_block *block = alloc->first;
	while (block) {
		alloc_block *next = block->next_block;
		core_alloc_block_free(block);
		block = next;
	}
}