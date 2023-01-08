#ifndef CORE_ALLOCATION_BLOCK_H
#define CORE_ALLOCATION_BLOCK_H

#include <stdint.h>

typedef struct {
	int id;
	int type;
	int size;
	int allocator_id;
	int block_num;
} alloc_info;

struct alloc_block;
typedef struct alloc_block {
	uint8_t *mem_start;
	alloc_info *memory_metadata;
	int allocated_bytes;
	int allocated_objects;
	int length;
	struct alloc_block *next_block;
} alloc_block;

int core_alloc_block_align_size(int size);

alloc_block *core_alloc_block_init(int size);

void *core_alloc_block_alloc(alloc_block *block, int size, int type);

alloc_info *core_alloc_block_get_metadata(alloc_block *block, void *ptr);

void core_alloc_block_free(alloc_block *block);

#endif // CORE_ALLOCATION_BLOCK_H
