#include "arena_allocator.h"

#include <stdlib.h>
#include <string.h>

arena_allocator *core_arena_allocator_init(int size)
{
	if (size < sizeof(arena_allocator))
		return NULL;
	uint8_t *memory = (uint8_t *)malloc(size);
	memset(memory, 0, size);
	arena_allocator *arena = (arena_allocator *)memory;
	arena->start = memory;
	arena->next = memory + sizeof(arena_allocator);
	arena->allocated_bytes = sizeof(arena_allocator);
	arena->length = size;
	return arena;
}

void *core_arena_allocator_alloc(arena_allocator *arena, int size)
{
	if (arena->allocated_bytes + size > arena->length)
		return NULL;
	void *memory = arena->next;
	arena->next += size;
	arena->allocated_bytes += size;
	return memory;
}

void core_arena_allocator_free(arena_allocator *arena)
{
	free(arena->start);
}