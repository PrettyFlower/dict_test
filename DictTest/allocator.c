#include "allocator.h"

allocator *core_allocator_init(int size)
{
	if (size < sizeof(alloc_block) + sizeof(allocator))
		return NULL;
	alloc_block *first = core_alloc_block_init(size);
	allocator *alloc = core_alloc_block_alloc(first, sizeof(allocator));
	alloc->block_size = size;
	alloc->first = first;
	alloc->last = first;
	alloc->num_blocks = 1;
	return alloc;
}

void *core_allocator_alloc(allocator *alloc, int size)
{
	if (size > alloc->block_size)
		return NULL;

	if (alloc->last->allocated_bytes + size > alloc->last->length) {
		alloc_block *next = core_alloc_block_init(alloc->block_size);
		alloc->last->next_block = next;
		alloc->last = next;
		alloc->num_blocks++;
	}

	return core_alloc_block_alloc(alloc->last, size);
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