#ifndef CORE_ARENA_ALLOCATOR_H
#define CORE_ARENA_ALLOCATOR_H

#include "arena_allocator.h"

#include <stdint.h>

typedef struct {
	uint8_t *start;
	uint8_t *next;
	int allocated_bytes;
	int length;
} arena_allocator;

arena_allocator *core_arena_allocator_init(int size);

void *core_arena_allocator_alloc(arena_allocator *arena, int size);

void core_arena_allocator_free(arena_allocator *arena);

#endif // CORE_ARENA_ALLOCATOR_H
