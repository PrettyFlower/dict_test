#include "allocator_pool.h"

#include "allocator.h"
#include "dict.h"

#include <stdint.h>

static struct {
	int next_allocator_id;
	allocator allocators[100];
	allocator *ptr_infos_allocator;
	dictionary *ptr_infos;
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
	data.ptr_infos_allocator = core_allocator_pool_create_allocator(10000000);
	data.ptr_infos_allocator->track = 0;
	data.ptr_infos = core_dict_init(data.ptr_infos_allocator, 1000000, ptr_hash, ptr_equals);
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

void core_allocator_pool_add_ptr(void *ptr, int allocator_id, int block_num, int offset)
{
	// happens when we first allocate the ptr_infos dictionary
	if (data.ptr_infos == NULL)
		return;

	ptr_info *info = core_allocator_alloc(data.ptr_infos_allocator, sizeof(ptr_info));
	info->ptr = ptr;
	info->allocator_id = allocator_id;
	info->block_num = block_num;
	info->offset = offset;
	core_dict_add(data.ptr_infos, ptr, info);
}

ptr_info *core_allocator_pool_get_ptr_info(void *ptr)
{
	return core_dict_get(data.ptr_infos, ptr);
}

void core_allocator_pool_remove_ptr(void *ptr)
{
	ptr_info *info = core_dict_remove(data.ptr_infos, ptr);
}
