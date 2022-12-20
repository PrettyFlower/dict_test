#include "allocator_pool.h"

#include "allocator.h"
#include "dict.h"

#include <stdint.h>

static struct {
	int next_allocator_id;
	allocator allocators[100];
} data;

static uint32_t ptr_hash(void *key)
{
	return (uint32_t)((size_t)key % UINT32_MAX);
}

int ptr_equals(void *a, void *b)
{
	return a == b;
}

void core_allocator_pool_init(void)
{
	data.next_allocator_id = 0;
}

allocator *core_allocator_pool_create_allocator(int size)
{
	allocator *alloc = core_allocator_init(data.next_allocator_id, size);
	data.next_allocator_id++;
	return alloc;
}

allocator *core_allocator_pool_get(int id)
{
	return &data.allocators[id];
}
