#include "dict.h"

#include "string.h"

#include <stdio.h>

static clock_t elapsed1;
static clock_t elapsed2;

clock_t get_elapsed1()
{
	return elapsed1;
}

clock_t get_elapsed2()
{
	return elapsed2;
}

dictionary *core_dict_init(arena_allocator *arena, int capacity, uint32_t(*get_hash)(void *key), int(*equals)(void *a, void *b))
{
	elapsed1 = 0;
	elapsed2 = 0;
	dictionary *dict = core_arena_allocator_alloc(arena, sizeof(dictionary));
	dict->allocator = arena;
	dict->length = 0;
	dict->kvps = core_arena_allocator_alloc(arena, sizeof(dictionary_kvp) * capacity);
	dict->capacity = capacity;
	dict->buckets = core_arena_allocator_alloc(arena, sizeof(dictionary_kvp *) * 108631);
	dict->num_buckets = 108631;
	dict->first = NULL;
	dict->get_hash = get_hash;
	dict->equals = equals;
	return dict;
}

void core_dict_add(dictionary *dict, void *key, void *value)
{
	//clock_t start = clock();
	uint32_t hash = dict->get_hash(key);
	int bucket = hash % dict->num_buckets;
	dictionary_kvp *existing = dict->buckets[bucket];
	while (existing) {
		// we already have a matching key, so just update the value of the KVP and return
		if (existing->hash == hash && dict->equals(existing->key, key)) {
			existing->value = value;
			return;
		}
		existing = existing->bucket_next;
	}
	dictionary_kvp *kvp = &dict->kvps[dict->length];
	kvp->key = key;
	kvp->value = value;
	kvp->hash = hash;
	kvp->bucket_next = dict->buckets[bucket];
	dict->buckets[bucket] = kvp;
	dict->length++;
	//elapsed1 += clock() - start;
}

void *core_dict_get(dictionary *dict, void *key)
{
	//clock_t start = clock();
	uint32_t hash = dict->get_hash(key);
	int bucket = hash % dict->num_buckets;
	dictionary_kvp *kvp = dict->buckets[bucket];
	while (kvp != NULL) {
		if (kvp->hash == hash && dict->equals(key, kvp->key)) {
			//elapsed2 += clock() - start;
			return kvp->value;
		}
		kvp = kvp->bucket_next;
	}
	//elapsed2 += clock() - start;
	return NULL;
}