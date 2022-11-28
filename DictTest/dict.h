#ifndef CORE_DICTIONARY_H
#define CORE_DICTIONARY_H

#include "allocator.h"

#include <stdint.h>
#include <time.h>

struct dictionary_kvp;
typedef struct dictionary_kvp {
	void *key;
	void *value;
	uint32_t hash;
	struct dictionary_kvp *bucket_next;
	struct dictionary_kvp *ordered_next;
	struct dictionary_kvp *ordered_prev;
} dictionary_kvp;

typedef struct {
	allocator *allocator;
	int capacity;
	int length;
	dictionary_kvp **buckets;
	int num_buckets;
	dictionary_kvp *first;
	dictionary_kvp *last;
	dictionary_kvp *removed_kvp;
	uint32_t(*get_hash)(void *key);
	int(*equals)(void *a, void *b);
} dictionary;

dictionary *core_dict_init(allocator *alloc, int num_buckets, uint32_t(*get_hash)(void *key), int(*equals)(void *a, void *b));

void core_dict_add(dictionary *dict, void *key, void *value);

void *core_dict_get(dictionary *dict, void *key);

void *core_dict_remove(dictionary *dict, void *key);

void core_dict_iterate(dictionary *dict, void *ctx, void(*callback)(void *ctx, dictionary_kvp *kvp));

uint32_t core_dict_string_hash(void *key);

int core_dict_string_equals(void *a, void *b);

#endif // CORE_DICTIONARY_H
