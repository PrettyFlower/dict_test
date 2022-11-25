#include "dict.h"

#include "string.h"

#include <stdio.h>

dictionary *core_dict_init(arena_allocator *arena, int num_buckets, uint32_t(*get_hash)(void *key), int(*equals)(void *a, void *b))
{
	dictionary *dict = core_arena_allocator_alloc(arena, sizeof(dictionary));
	dict->allocator = arena;
	dict->length = 0;
	dict->buckets = core_arena_allocator_alloc(arena, sizeof(dictionary_kvp *) * num_buckets);
	dict->num_buckets = num_buckets;
	dict->first = NULL;
	dict->get_hash = get_hash;
	dict->equals = equals;
	return dict;
}

void core_dict_add(dictionary *dict, void *key, void *value)
{
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
	dictionary_kvp *kvp = core_arena_allocator_alloc(dict->allocator, sizeof(dictionary_kvp));
	kvp->key = key;
	kvp->value = value;
	kvp->hash = hash;
	kvp->bucket_next = dict->buckets[bucket];
	dict->buckets[bucket] = kvp;
	dict->length++;
}

void *core_dict_get(dictionary *dict, void *key)
{
	uint32_t hash = dict->get_hash(key);
	int bucket = hash % dict->num_buckets;
	dictionary_kvp *kvp = dict->buckets[bucket];
	while (kvp != NULL) {
		if (kvp->hash == hash && dict->equals(key, kvp->key)) {
			return kvp->value;
		}
		kvp = kvp->bucket_next;
	}
	return NULL;
}