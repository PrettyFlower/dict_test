#include "csharp_random.h"

#include <limits.h>
#include <stdlib.h>

const int MBIG = INT_MAX;
const int MSEED = 161803398;
const int MZ = 0;

csharp_random *core_csharp_random_init(arena_allocator *arena, int seed)
{
	csharp_random *r = core_arena_allocator_alloc(arena, sizeof(csharp_random));
	int subtraction = seed == INT_MIN ? INT_MAX : abs(seed);
	int mj = MSEED - subtraction;
	r->seed_array[55] = mj;
	int mk = 1;
	for (int i = 1; i < 55; i++) {
		int ii = (21 * i) % 55;
		r->seed_array[ii] = mk;
		mk = mj - mk;
		if (mk < 0)
			mk += MBIG;
		mj = r->seed_array[ii];
	}
	for (int k = 1; k < 5; k++) {
		for (int i = 1; i < 56; i++) {
			r->seed_array[i] -= r->seed_array[1 + (i + 30) % 55];
			if (r->seed_array[i] < 0)
				r->seed_array[i] += MBIG;
		}
	}
	r->inext = 0;
	r->inextp = 21;
	return r;
}

int core_csharp_random_next(csharp_random *r)
{
	int loc_i_next = r->inext;
	int loc_i_nextp = r->inextp;

	if (++loc_i_next >= 56) loc_i_next = 1;
	if (++loc_i_nextp >= 56) loc_i_nextp = 1;

	int ret_val = r->seed_array[loc_i_next] - r->seed_array[loc_i_nextp];

	if (ret_val == MBIG) ret_val--;
	if (ret_val < 0) ret_val += MBIG;

	r->seed_array[loc_i_next] = ret_val;

	r->inext = loc_i_next;
	r->inextp = loc_i_nextp;

	return ret_val;
}