#include "string.h"

#include "murmur_hash_3.h"

string *core_string_init(arena_allocator *arena, int data_length)
{
	string *s = core_arena_allocator_alloc(arena, sizeof(string));
	if (s == NULL)
		return NULL;
	s->data = core_arena_allocator_alloc(arena, data_length);
	if (s->data == NULL)
		return NULL;
	s->length = 0;
	s->data_length = data_length;
	return s;
}

string *core_string_init_from_c_string(arena_allocator *arena, char *c, int max_length)
{
	string *s = core_string_init(arena, max_length);
	core_string_copy_c_string(s, c, max_length);
	return s;
}

/// <summary>
/// 
/// </summary>
/// <param name="s"></param>
/// <param name="c"></param>
/// <param name="length">Assume the length does NOT include the NULL terminator character, i.e. they called strlen().</param>
void core_string_copy_c_string(string *s, char *c, int length)
{
	int max_copy_length = length;
	if (length > s->data_length - 1)
		length = s->data_length - 1;
	int new_length = 0;
	for (; new_length < max_copy_length; new_length++) {
		char next = c[new_length];
		if (next == '\0')
			break;
		s->data[new_length] = next;
	}
	s->data[new_length] = '\0';
	s->length = new_length;
}

int core_string_equals(string *a, string *b)
{
	if (a == b)
		return 1;
	if (a->length != b->length)
		return 0;
	if (core_string_hash(a) != core_string_hash(b))
		return 0;
	for (int i = 0; i < a->length; i++) {
		if (a->data[i] != b->data[i])
			return 0;
	}
	return 1;
}

uint32_t core_string_hash(string *s)
{
	return murmur_hash_3_x86_32(s->data, s->length, 1);
}