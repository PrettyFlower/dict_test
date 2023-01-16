#include "rh_dict.h"

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

/*static void resize(rh_dict *dict) {
	uint32_t new_capacity = 2 * dict->capacity;
	new_capacity = get_num_buckets(new_capacity);
	if (new_capacity <= dict->capacity)
		return;

	rh_dict_kvp **new_buckets = core_allocator_alloc(dict->allocator, sizeof(rh_dict_kvp *) * new_capacity);

	rh_dict_kvp *next = dict->first;
	while (next) {
		int new_bucket = next->hash % new_capacity;
		next->bucket_next = new_buckets[new_bucket];
		new_buckets[new_bucket] = next;
		next = next->ordered_next;
	}

	dict->buckets = new_buckets;
	dict->capacity = new_capacity;
	dict->num_buckets = new_capacity;
}*/

rh_dict *core_rh_dict_init(allocator *alloc, uint32_t initial_capacity, uint32_t(*get_hash)(void *key), int(*equals)(void *a, void *b))
{
	rh_dict *dict = core_allocator_alloc(alloc, sizeof(rh_dict));
	dict->allocator = alloc;
	dict->capacity = initial_capacity > 0 ? initial_capacity : 1;
	dict->length = 0;
	dict->max_psl = 0;
	dict->num_kvps = get_num_buckets(initial_capacity);
	dict->kvps = core_allocator_alloc(dict->allocator, sizeof(rh_dict_kvp) * dict->num_kvps);
	//dict->first = NULL;
	//dict->last = NULL;
	dict->get_hash = get_hash;
	dict->equals = equals;
	return dict;
}

void core_rh_dict_add(rh_dict *dict, void *key, void *value)
{
	//if (dict->length >= dict->capacity)
		//resize(dict);

	rh_dict_kvp *get_kvp = core_rh_dict_get(dict, key);
	if (get_kvp) {
		get_kvp->value = value;
		return;
	}

	uint32_t hash = dict->get_hash(key);
	int index = hash % dict->num_kvps;
	rh_dict_kvp move_kvp = {
		.key = key,
		.value = value,
		.hash = hash,
		.psl = 0,
		.in_use = 1,
		//.ordered_next = NULL,
		//.ordered_prev = dict->last
	};

	rh_dict_kvp *check_kvp = &dict->kvps[index];
	while (check_kvp->in_use) {
		if (move_kvp.psl > check_kvp->psl) {
			rh_dict_kvp new_move_kvp = *check_kvp;
			memcpy(check_kvp, &move_kvp, sizeof(rh_dict_kvp));
			move_kvp = new_move_kvp;
		}
		index++;
		index %= dict->num_kvps;
		check_kvp = &dict->kvps[index];
		move_kvp.psl++;
		if (move_kvp.psl > dict->max_psl)
			dict->max_psl = move_kvp.psl;
	}

	memcpy(check_kvp, &move_kvp, sizeof(rh_dict_kvp));

	// update the sorted linked list
	//if (dict->last != NULL)
		//dict->last->ordered_next = check_kvp;

	// update the dictionary values
	//if (dict->first == NULL)
		//dict->first = check_kvp;
	//dict->last = check_kvp;
	dict->length++;
}

void *core_rh_dict_get(rh_dict *dict, void *key)
{
	uint32_t hash = dict->get_hash(key);
	int start_idx = hash % dict->num_kvps;
	for (int i = 0; i < dict->max_psl + 1; i++) {
		rh_dict_kvp *kvp = &dict->kvps[(start_idx + i) % dict->num_kvps];
		// find the start of the probing sequence
		if (kvp->in_use && kvp->hash != hash)
			continue;
		// hit the end of the probing sequence, we don't have the element
		if (!kvp->in_use || kvp->psl > i)
			return NULL;
		// we found it!
		if (kvp->hash == hash && dict->equals(key, kvp->key))
			return kvp->value;
	}
	return NULL;
}

/*void *core_rh_dict_remove(rh_dict *dict, void *key)
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

void core_rh_dict_iterate(rh_dict *dict, void *ctx, void(*callback)(void *ctx, rh_dict_kvp *kvp))
{
	rh_dict_kvp *next = dict->first;
	while (next) {
		callback(ctx, next);
		next = next->ordered_next;
	}
}*/

uint32_t core_rh_dict_free_spaces(rh_dict *dict)
{
	uint32_t sum = 0;
	for (int i = 0; i < dict->num_kvps; i++) {
		rh_dict_kvp *kvp = &dict->kvps[i];
		if (!kvp->in_use)
			sum++;
	}
	return sum;
}