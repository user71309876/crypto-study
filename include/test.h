/*
 * test.h - A very small test framework
 *
 * Usage:
 *   CHECK_INT(actual, expected);
 *   CHECK_STR(actual_string, expected_string);
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

#define TEST_SECTION(name) printf("- %s\n", name)

#define TEST_SUMMARY() (                                                   \
    printf("\nResult: %d passed, %d failed\n", g_test_pass, g_test_fail),  \
    (g_test_fail == 0 ? 0 : 1))

#endif /* TEST_H */
