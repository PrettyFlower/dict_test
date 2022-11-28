#include "allocation_block.h"

#include <stdlib.h>
#include <string.h>

alloc_block *core_alloc_block_init(int size)
{
	if (size < sizeof(alloc_block))
		return NULL;
	uint8_t *memory = (uint8_t *)malloc(size);
	memset(memory, 0, size);
	alloc_block *block = (alloc_block *)memory;
	block->mem_start = memory;
	block->next_mem = memory + sizeof(alloc_block);
	block->allocated_bytes = sizeof(alloc_block);
	block->length = size;
	block->next_block = NULL;
	return block;
}

void *core_alloc_block_alloc(alloc_block *block, int size)
{
	if (block->allocated_bytes + size > block->length)
		return NULL;
	void *memory = block->next_mem;
	block->next_mem += size;
	block->allocated_bytes += size;
	return memory;
}

void core_alloc_block_free(alloc_block *block)
{
	free(block->mem_start);
}