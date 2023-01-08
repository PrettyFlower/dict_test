#include "allocation_block.h"

#include "object_types.h"

#include <stdlib.h>

int core_alloc_block_align_size(int size)
{
	// apparently it is undefined behavior if your pointers aren't aligned based on size?
	int ptr_size = sizeof(void *);
	int align_size = (size + ptr_size - 1) & -ptr_size;
	return align_size;
	//return size;
}

alloc_block *core_alloc_block_init(int size)
{
	int alloc_block_size = core_alloc_block_align_size(sizeof(alloc_block));
	if (size < alloc_block_size)
		return NULL;
	uint8_t *memory = (uint8_t *)calloc(1, size);
	if (memory == NULL)
		return NULL;
	alloc_info *memory_metadata = (alloc_info *)calloc(size / sizeof(void *), sizeof(alloc_info));
	if (memory_metadata == NULL) {
		free(memory);
		return NULL;
	}
	alloc_block *block = (alloc_block *)memory;
	block->mem_start = memory;
	block->memory_metadata = memory_metadata;
	block->allocated_bytes = alloc_block_size;
	block->length = size;
	block->next_block = NULL;
	return block;
}

void *core_alloc_block_alloc(alloc_block *block, int size, int type)
{
	int aligned_size = core_alloc_block_align_size(size + 4);
	if (block->allocated_bytes + aligned_size > block->length)
		return NULL;
	uint8_t *memory = block->mem_start + block->allocated_bytes;
	*memory = block->allocated_objects;
	memory += 4;
	block->allocated_bytes += aligned_size;
	alloc_info *info = &block->memory_metadata[block->allocated_objects];
	info->id = block->allocated_objects + 1;
	info->type = type;
	info->size = size;
	block->allocated_objects += 1;
	return memory;
}

alloc_info *core_alloc_block_get_metadata(alloc_block *block, void *ptr)
{
	int meta_offset = *((uint8_t *)ptr - 4);
	alloc_info *info = &block->memory_metadata[meta_offset];
	return info;
}

void core_alloc_block_free(alloc_block *block)
{
	free(block->mem_start);
	free(block->memory_metadata);
}