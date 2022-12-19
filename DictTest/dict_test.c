#include "dict.h"

#include "allocator_pool.h"
#include "dict.h"
#include "mtwister.h"
#include "prime_utils.h"
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

string *rand_string(allocator *alloc, mt_rand *r)
{
    string *s = core_string_init(alloc, 11);
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

static void iterate_test(void *ctx, dictionary_kvp *kvp)
{
    iterate_ctx *it_ctx = (iterate_ctx *)ctx;
    string *s = it_ctx->strings[it_ctx->idx];
    string *s2 = (string *)kvp->value;
    if (!core_string_equals(s, s2))
        printf("Strings out of order at idx %d\n", it_ctx->idx);
    it_ctx->idx++;
}

static void run_pass()
{
    clock_t total_start = clock();
    clock_t local_start = clock();
    int num_strings = 100000;
    mt_rand r = seed_rand(time(NULL));
    allocator *alloc = core_allocator_pool_create_allocator(num_strings * 25);
    dictionary *dict = core_dict_init(alloc, 0, core_dict_string_hash, core_dict_string_equals);
    string **strings = core_allocator_alloc(alloc, sizeof(string *) * num_strings);
    for (int i = 0; i < num_strings; i++) {
        string *s = rand_string(alloc, &r);
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
    allocator *alloc2 = core_allocator_pool_create_allocator(num_strings * 10);
    string **new_strings = core_allocator_alloc(alloc, sizeof(string *) * num_strings);
    int str_count = 0;
    for (int i = 0; i < num_strings; i++) {
        string *s = strings[i];
        if (s != NULL) {
            new_strings[str_count] = s;
            str_count++;
        }
    }
    for (int i = str_count; i < num_strings; i++) {
        string *s = rand_string(alloc, &r);
        new_strings[i] = s;
        core_dict_add(dict, s, s);
    }
    memcpy(strings, new_strings, sizeof(string *) * num_strings);
    core_allocator_free_all(alloc2);
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
    core_allocator_free_all(alloc);
}

int main()
{
    core_allocator_pool_init();
    //allocator *alloc = core_allocator_pool_create_allocator(1000);
    /*string *s = core_string_init(alloc, 10);
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

    /*setlocale(LC_ALL, "");
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
    printf("%s\n", buffer);

    time_t time_val;
    time(&time_val);
    struct tm time_info;
    localtime_s(&time_info, &time_val);
    memset(buffer, 0, 50);
    strftime(buffer, 50, "%c", &time_info);
    printf("%s\n", buffer);*/

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
