#include "dict.h"

#include "string.h"

#include <stdio.h>
#include <string.h>

dictionary *core_dict_init(arena_allocator *arena, int num_buckets, uint32_t(*get_hash)(void *key), int(*equals)(void *a, void *b))
{
	dictionary *dict = core_arena_allocator_alloc(arena, sizeof(dictionary));
	dict->allocator = arena;
	dict->length = 0;
	dict->buckets = core_arena_allocator_alloc(arena, sizeof(dictionary_kvp *) * num_buckets);
	dict->num_buckets = num_buckets;
	dict->first = NULL;
	dict->last = NULL;
	dict->removed_kvp = NULL;
	dict->get_hash = get_hash;
	dict->equals = equals;
	return dict;
}

void core_dict_add(dictionary *dict, void *key, void *value)
{
	// find the right hash bucket with the linked list of KVPs
	uint32_t hash = dict->get_hash(key);
	int bucket = hash % dict->num_buckets;
	dictionary_kvp *existing = dict->buckets[bucket];

	// find the end of the linked list
	while (existing) {
		// we already have a matching key, so just update the value of the KVP and return
		if (existing->hash == hash && dict->equals(existing->key, key)) {
			existing->value = value;
			return;
		}
		existing = existing->bucket_next;
	}

	// get a new KVP to store the data
	dictionary_kvp *kvp;
	// check to see if we have any deleted KVPs hanging around we can re-use
	if (dict->removed_kvp) {
		kvp = dict->removed_kvp;
		dict->removed_kvp = kvp->bucket_next;
	} else {
		kvp = core_arena_allocator_alloc(dict->allocator, sizeof(dictionary_kvp));
	}
	kvp->key = key;
	kvp->value = value;
	kvp->hash = hash;

	// update the bucket linked list
	kvp->bucket_next = dict->buckets[bucket];
	dict->buckets[bucket] = kvp;

	// update the sorted linked list
	if (dict->last != NULL)
		dict->last->ordered_next = kvp;
	kvp->ordered_prev = dict->last;

	// update the dictionary values
	if (dict->first == NULL)
		dict->first = kvp;
	dict->last = kvp;
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

void *core_dict_remove(dictionary *dict, void *key)
{
	uint32_t hash = dict->get_hash(key);
	int bucket = hash % dict->num_buckets;
	dictionary_kvp *kvp = dict->buckets[bucket];
	dictionary_kvp *last_bucket_kvp = NULL;
	while (kvp != NULL) {
		if (kvp->hash == hash && dict->equals(key, kvp->key)) {
			void *value = kvp->value;

			// remove from bucket and reconnect the linked list
			if (last_bucket_kvp)
				last_bucket_kvp->bucket_next = kvp->bucket_next;
			else
				dict->buckets[bucket] = kvp->bucket_next;

			// update dict values
			if (dict->first == kvp)
				dict->first = kvp->ordered_next;
			if (dict->last == kvp)
				dict->last = kvp->ordered_prev;
			dict->length--;

			// remove from ordered linked list and reconnect it
			if (kvp->ordered_next)
				kvp->ordered_next->ordered_prev = kvp->ordered_prev;
			if (kvp->ordered_prev)
				kvp->ordered_prev->ordered_next = kvp->ordered_next;

			// zero out and add to list of removed KVPs
			memset(kvp, 0, sizeof(dictionary_kvp));
			kvp->bucket_next = dict->removed_kvp;
			dict->removed_kvp = kvp;
			return value;
		}
		last_bucket_kvp = kvp;
		kvp = kvp->bucket_next;
	}
	return NULL;
}

void core_dict_iterate(dictionary *dict, void *ctx, void(*callback)(void *ctx, dictionary_kvp *kvp))
{
	dictionary_kvp *next = dict->first;
	while (next) {
		callback(ctx, next);
		next = next->ordered_next;
	}
}