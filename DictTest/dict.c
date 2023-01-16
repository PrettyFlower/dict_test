#include "dict.h"

#include "prime_utils.h"
#include "string.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int get_num_buckets(int capacity)
{
	if (capacity >= UINT32_MAX)
		return UINT32_MAX;
	int num_buckets = core_prime_utils_get_next_prime(capacity);
	return num_buckets;
}

static void resize(dictionary *dict) {
	uint32_t new_capacity = 2 * dict->capacity;
	new_capacity = get_num_buckets(new_capacity);
	if (new_capacity <= dict->capacity)
		return;

	dictionary_kvp **new_buckets = core_allocator_alloc(dict->allocator, sizeof(dictionary_kvp *) * new_capacity);

	dictionary_kvp *next = dict->first;
	while (next) {
		int new_bucket = next->hash % new_capacity;
		next->bucket_next = new_buckets[new_bucket];
		new_buckets[new_bucket] = next;
		next = next->ordered_next;
	}

	dict->buckets = new_buckets;
	dict->capacity = new_capacity;
	dict->num_buckets = new_capacity;
}

dictionary *core_dict_init(allocator *alloc, uint32_t initial_capacity, uint32_t(*get_hash)(void *key), int(*equals)(void *a, void *b))
{
	dictionary *dict = core_allocator_alloc(alloc, sizeof(dictionary));
	dict->allocator = alloc;
	dict->capacity = initial_capacity > 0 ? initial_capacity : 1;
	dict->length = 0;
	dict->longest_chain = 0;
	dict->num_buckets = get_num_buckets(initial_capacity);
	dict->buckets = core_allocator_alloc(dict->allocator, sizeof(dictionary_kvp *) * dict->num_buckets);
	dict->first = NULL;
	dict->last = NULL;
	dict->removed_kvp = NULL;
	dict->get_hash = get_hash;
	dict->equals = equals;
	return dict;
}

void core_dict_add(dictionary *dict, void *key, void *value)
{
	if(dict->length >= dict->capacity)
		resize(dict);

	// find the right hash bucket with the linked list of KVPs
	uint32_t hash = dict->get_hash(key);
	int bucket = hash % dict->num_buckets;
	dictionary_kvp *existing = dict->buckets[bucket];

	// find the end of the linked list. length starts at one to count the new element we are trying to insert
	uint32_t linkedlist_length = 1;
	while (existing) {
		// we already have a matching key, so just update the value of the KVP and return
		if (existing->hash == hash && dict->equals(existing->key, key)) {
			existing->value = value;
			return;
		}
		existing = existing->bucket_next;
		linkedlist_length++;
	}

	// get a new KVP to store the data
	dictionary_kvp *kvp;
	// check to see if we have any deleted KVPs hanging around we can re-use
	if (dict->removed_kvp) {
		kvp = dict->removed_kvp;
		dict->removed_kvp = kvp->bucket_next;
	} else {
		kvp = core_allocator_alloc(dict->allocator, sizeof(dictionary_kvp));
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
	if (linkedlist_length > dict->longest_chain)
		dict->longest_chain = linkedlist_length;
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

uint32_t core_dict_string_hash(void *key)
{
	string *s = (string *)key;
	return core_string_hash(s);
}

int core_dict_string_equals(void *a, void *b)
{
	string *s1 = (string *)a;
	string *s2 = (string *)b;
	return core_string_equals(s1, s2);
}