#ifndef CORE_MTWISTER_H
#define CORE_MTWISTER_H

#include <stdint.h>

#define STATE_VECTOR_LENGTH 624
#define STATE_VECTOR_M      397 /* changes to STATE_VECTOR_LENGTH also require changes to this */

typedef struct {
	uint32_t mt[STATE_VECTOR_LENGTH];
	int index;
} mt_rand;

mt_rand seed_rand(uint32_t seed);
uint32_t gen_rand(mt_rand *rand);
double gen_rand_double(mt_rand *rand);

#endif // CORE_ALLOCATOR_H
