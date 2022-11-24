#ifndef CORE_MURMURHASH3_H
#define CORE_MURMURHASH3_H

#include <stdint.h>

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the
// public domain. The author hereby disclaims copyright to this source
// code.

uint32_t murmur_hash_3_x86_32(const void *key, int len, uint32_t seed);

void murmur_hash_3_x86_128(const void *key, int len, uint32_t seed, void *out);

void murmur_hash_3_x64_128(const void *key, int len, uint32_t seed, void *out);

#endif // CORE_MURMURHASH3_H
