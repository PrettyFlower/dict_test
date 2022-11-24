#ifndef CORE_CSHARP_RANDOM_H
#define CORE_CSHARP_RANDOM_H

#include "arena_allocator.h"

typedef struct {
	int inext;
	int inextp;
	int seed_array[56];
} csharp_random;

csharp_random *core_csharp_random_init(arena_allocator *arena, int seed);

int core_csharp_random_next(csharp_random *r);

#endif // CORE_CSHARP_RANDOM_H
