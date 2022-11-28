#ifndef CORE_STRING_H
#define CORE_STRING_H

#include "allocator.h"

#include <stdint.h>

typedef struct {
	char *data;
	int length;
	int data_length;
} string;

string *core_string_init(allocator *alloc, int data_length);

string *core_string_init_from_c_string(allocator *alloc, char *c, int max_length);

void core_string_copy_c_string(string *s, char *c, int length);

int core_string_equals(string *a, string *b);

uint32_t core_string_hash(string *s);

#endif // CORE_STRING_H
