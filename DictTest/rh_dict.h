#ifndef CORE_RH_DICTIONARY_H
#define CORE_RH_DICTIONARY_H

#include "allocator.h"

#include <stdint.h>

struct rh_dict_kvp;
typedef struct rh_dict_kvp {
	void *key;
	void *value;
	uint32_t hash;
	uint32_t psl;
	char in_use;
	//struct rh_dict_kvp *ordered_next;
	//struct rh_dict_kvp *ordered_prev;
} rh_dict_kvp;

typedef struct {
	allocator *allocator;
	uint32_t capacity;
	uint32_t length;
	uint32_t max_psl;
	rh_dict_kvp *kvps;
	int num_kvps;
	//rh_dict_kvp *first;
	//rh_dict_kvp *last;
	uint32_t(*get_hash)(void *key);
	int(*equals)(void *a, void *b);
} rh_dict;

rh_dict *core_rh_dict_init(allocator *alloc, uint32_t num_buckets, uint32_t(*get_hash)(void *key), int(*equals)(void *a, void *b));

void core_rh_dict_add(rh_dict *dict, void *key, void *value);

void *core_rh_dict_get(rh_dict *dict, void *key);

uint32_t core_rh_dict_free_spaces(rh_dict *dict);

//void *core_rh_dict_remove(rh_dict *dict, void *key);

//void core_rh_dict_iterate(rh_dict *dict, void *ctx, void(*callback)(void *ctx, rh_dict_kvp *kvp));

#endif // CORE_RH_DICTIONARY_H
