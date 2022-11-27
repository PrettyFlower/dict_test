#include "dict.h"

#include"csharp_random.h"
#include "dict.h"
#include "mtwister.h"
#include "string.h"
#include "utf8proc.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

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
        printf("Strings out of order at idx %d\n", ctx->idx);
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
    for (int i = 0; i < num_strings / 5; i++) {
        int idx = gen_rand(&r) % num_strings;
        string *s = strings[idx];
        if (s == NULL)
            continue;
        string *s2 = core_dict_remove(dict, s);
        if (!core_string_equals(s, s2))
            printf("Whoops...\n");
        strings[idx] = NULL;
    }
    clock_t removal_time = clock() - local_start;
    printf("removal: %ld\n", removal_time);

    local_start = clock();
    arena_allocator *arena2 = core_arena_allocator_init(num_strings * 10);
    string **new_strings = core_arena_allocator_alloc(arena2, sizeof(string *) * num_strings);
    int str_count = 0;
    for (int i = 0; i < num_strings; i++) {
        string *s = strings[i];
        if (s != NULL) {
            new_strings[str_count] = s;
            str_count++;
        }
    }
    for (int i = str_count; i < num_strings; i++) {
        string *s = rand_string(arena, &r);
        new_strings[i] = s;
        core_dict_add(dict, s, s);
    }
    memcpy(strings, new_strings, sizeof(string *) * num_strings);
    core_arena_allocator_free(arena2);
    clock_t compress_time = clock() - local_start;
    printf("compress and add new: %ld\n", compress_time);

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
    arena_allocator *arena = core_arena_allocator_init(1000);
    /*string *s = core_string_init(arena, 10);
    char *src = "💩";
    int len = strlen(src);
    utf8proc_int32_t pts[10];
    utf8proc_size_t ret = utf8proc_decompose(src, len, pts, 10, 0);
    wchar_t *mode = L"wb, ccs=UTF-8";
    char *filename = "💩.txt";
    wchar_t wide_filename[10];
    memset(wide_filename, 0, sizeof(wchar_t) * 10);
    int filename_len = strlen(filename);
    int result = MultiByteToWideChar(CP_UTF8, 0, filename, filename_len, wide_filename, 10);
    //char *filename = "test.txt";
    FILE *f;
    errno_t e = _wfopen_s(&f, wide_filename, mode);
    fwrite(src, 1, len, f);
    fclose(f);*/
    
    //utf32_to_utf16(pts[0], output);
    

    /*int32_t p_dest_len = 0;
    UErrorCode err = U_ZERO_ERROR;
    u_strFromUTF8Lenient(s->data, s->data_length, &p_dest_len, src, -1, &err);
    int len = u_countChar32(s->data, -1);
    UChar32 test;
    U16_GET(s->data, 0, 1, len, test);*/

    setlocale(LC_ALL, "");
    const struct lconv *const currentlocale = localeconv();
    char buffer[50];
    //sprintf_s(buffer, 50, "%d", 5000);
    NUMBERFMT fmt = {
        .NumDigits = 0,
        .LeadingZero = 0,
        .Grouping = 3,
        .lpDecimalSep = ".",
        .lpThousandSep = ",",
        .NegativeOrder = 0
    };
    GetNumberFormat(NULL, NULL, "5000", &fmt, buffer, 50);

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
