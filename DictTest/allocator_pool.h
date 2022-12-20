#ifndef CORE_ALLOCATOR_POOL_H
#define CORE_ALLOCATOR_POOL_H

#include "allocator.h"

typedef struct {
	void *ptr;
	int allocator_id;
	int block_num;
	int offset;
} ptr_info;

void core_allocator_pool_init(void);

allocator *core_allocator_pool_create_allocator(int size);

allocator *core_allocator_pool_get(int id);

#endif // CORE_ALLOCATOR_POOL_H
