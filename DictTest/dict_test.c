#include "dict.h"

#include"csharp_random.h"
#include "dict.h"
#include "mtwister.h"
#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int num_strings;
    string **strings;
    int idx;
} iterate_ctx;

string *rand_string(arena_allocator *arena, mt_rand *r)
{
    string *s = core_string_init(arena, 11);
    for (int i = 0; i < 10; i++) {
        s->data[i] = (gen_rand(r) % 26) + 97;
    }
    s->data[10] = '\0';
    s->length = 10;
    //printf(c);
    //printf("\n");
    //int slen = strlen(c);
    return s;
}

static void iterate_test(iterate_ctx *ctx, dictionary_kvp *kvp)
{
    string *s = ctx->strings[ctx->idx];
    string *s2 = (string *)kvp->value;
    if (!core_string_equals(s, s2))
        printf("Strings out of order at idx %d", ctx->idx);
    ctx->idx++;
}

static void run_pass()
{
    clock_t total_start = clock();
    clock_t local_start = clock();
    int num_strings = 100000;
    mt_rand r = seed_rand(time(NULL));
    arena_allocator *arena = core_arena_allocator_init(num_strings * 100);
    //csharp_random *r = core_csharp_random_init(arena, time(NULL));
    dictionary *dict = core_dict_init(arena, num_strings, core_string_hash, core_string_equals);
    string **strings = core_arena_allocator_alloc(arena, sizeof(string *) * num_strings);
    for (int i = 0; i < num_strings; i++) {
        string *s = rand_string(arena, &r);
        strings[i] = s;
    }
    clock_t gen_time = clock() - local_start;
    printf("string generation: %ld\n", gen_time);

    local_start = clock();
    for (int i = 0; i < num_strings; i++) {
        string *s = strings[i];
        core_dict_add(dict, s, s);
    }
    clock_t add_time = clock() - local_start;
    printf("dict add: %ld\n", add_time);

    local_start = clock();
    for (int i = 0; i < num_strings; i++) {
        string *s = strings[i];
        string *s2 = core_dict_get(dict, s);
        if (!core_string_equals(s, s2))
            printf("Whoops...\n");
        //printf(s->data);
        //printf(", ");
        //printf(s2->data);
        //printf("\n");
    }
    clock_t lookup_compare_time = clock() - local_start;
    printf("lookup and compare: %ld\n", lookup_compare_time);

    local_start = clock();
    iterate_ctx iterate_ctx = {
        .num_strings = num_strings,
        .strings = strings,
        .idx = 0,
    };
    core_dict_iterate(dict, &iterate_ctx, iterate_test);
    clock_t iterate_time = clock() - local_start;
    printf("iterate: %ld\n", iterate_time);

    clock_t total_time = clock() - total_start;
    printf("total: %ld\n", total_time);
    core_arena_allocator_free(arena);
}

int main()
{
    run_pass();
    run_pass();

    char c;
    printf("Press any key to continue...\n");
#ifdef __MINGW32__
    scanf("%c", &c, 1);
#else
    scanf_s("%c", &c, 1);
#endif
    return 0;
}
