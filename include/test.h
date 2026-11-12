/*
 * test.h - A very small test framework
 *
 * Usage:
 *   CHECK_INT(actual, expected);
 *   CHECK_STR(actual_string, expected_string);
 *   CHECK_BYTES(actual_ptr, expected_ptr, length);   (added in Step 3)
 *   return TEST_SUMMARY();   // call at the end of main
 *
 * On failure it prints file:line with the expected and actual values.
 */
#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <string.h>

static int g_test_pass = 0;
static int g_test_fail = 0;

#define CHECK_INT(actual, expected) do {                                   \
    long long a_ = (long long)(actual), e_ = (long long)(expected);        \
    if (a_ == e_) { g_test_pass++; }                                       \
    else {                                                                 \
        g_test_fail++;                                                     \
        printf("  [FAIL] %s:%d  %s\n         expected: %lld\n         actual:   %lld\n", \
               __FILE__, __LINE__, #actual, e_, a_);                       \
    }                                                                      \
} while (0)

#define CHECK_STR(actual, expected) do {                                   \
    const char *a_ = (actual), *e_ = (expected);                           \
    if (a_ && e_ && strcmp(a_, e_) == 0) { g_test_pass++; }                \
    else {                                                                 \
        g_test_fail++;                                                     \
        printf("  [FAIL] %s:%d  %s\n         expected: \"%s\"\n         actual:   \"%s\"\n", \
               __FILE__, __LINE__, #actual, e_ ? e_ : "(null)",            \
               a_ ? a_ : "(null)");                                        \
    }                                                                      \
} while (0)

/* Print len bytes as hex, e.g. "0b 36 37 ..." */
static inline void test_print_hex_(const unsigned char *p, size_t len)
{
    for (size_t i = 0; i < len; i++)
        printf("%02x%s", p[i], (i + 1 < len) ? " " : "");
}

/* Compare two byte arrays. Unlike CHECK_STR, works with 0x00 bytes inside. */
#define CHECK_BYTES(actual, expected, len) do {                            \
    const unsigned char *a_ = (const unsigned char *)(actual);             \
    const unsigned char *e_ = (const unsigned char *)(expected);           \
    size_t n_ = (size_t)(len);                                             \
    if (a_ && e_ && memcmp(a_, e_, n_) == 0) { g_test_pass++; }            \
    else {                                                                 \
        g_test_fail++;                                                     \
        printf("  [FAIL] %s:%d  %s\n         expected: ", __FILE__, __LINE__, #actual); \
        if (e_) test_print_hex_(e_, n_); else printf("(null)");            \
        printf("\n         actual:   ");                                   \
        if (a_) test_print_hex_(a_, n_); else printf("(null)");            \
        printf("\n");                                                      \
    }                                                                      \
} while (0)

#define TEST_SECTION(name) printf("- %s\n", name)

#define TEST_SUMMARY() (                                                   \
    printf("\nResult: %d passed, %d failed\n", g_test_pass, g_test_fail),  \
    (g_test_fail == 0 ? 0 : 1))

#endif /* TEST_H */
