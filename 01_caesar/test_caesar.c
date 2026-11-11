/*
 * test_caesar.c - Caesar cipher tests (do not modify)
 */
#include "caesar.h"
#include "test.h"

int main(void)
{
    char buf[128];
    char buf2[128];

    printf("[Step 1] Caesar cipher tests\n");

    TEST_SECTION("Basic encryption");
    CHECK_INT(caesar_encrypt("ABC", buf, sizeof buf, 3), CRYPTO_OK);
    CHECK_STR(buf, "DEF");
    CHECK_INT(caesar_encrypt("hello", buf, sizeof buf, 1), CRYPTO_OK);
    CHECK_STR(buf, "ifmmp");

    TEST_SECTION("Wrap-around from end of alphabet to start");
    CHECK_INT(caesar_encrypt("XYZ", buf, sizeof buf, 3), CRYPTO_OK);
    CHECK_STR(buf, "ABC");
    CHECK_INT(caesar_encrypt("xyz", buf, sizeof buf, 3), CRYPTO_OK);
    CHECK_STR(buf, "abc");

    TEST_SECTION("Case preserved, non-letters unchanged");
    CHECK_INT(caesar_encrypt("Attack at Dawn! 123", buf, sizeof buf, 3), CRYPTO_OK);
    CHECK_STR(buf, "Dwwdfn dw Gdzq! 123");

    TEST_SECTION("Special key values");
    CHECK_INT(caesar_encrypt("Hello", buf, sizeof buf, 0), CRYPTO_OK);
    CHECK_STR(buf, "Hello");
    CHECK_INT(caesar_encrypt("Hello", buf, sizeof buf, 26), CRYPTO_OK);
    CHECK_STR(buf, "Hello");
    CHECK_INT(caesar_encrypt("abc", buf, sizeof buf, 29), CRYPTO_OK);   /* 29 = 3 */
    CHECK_STR(buf, "def");
    CHECK_INT(caesar_encrypt("abc", buf, sizeof buf, -1), CRYPTO_OK);   /* negative key */
    CHECK_STR(buf, "zab");
    CHECK_INT(caesar_encrypt("abc", buf, sizeof buf, -27), CRYPTO_OK);  /* -27 = -1 */
    CHECK_STR(buf, "zab");

    TEST_SECTION("ROT13: applying twice gives the original");
    CHECK_INT(caesar_encrypt("Hello, World", buf, sizeof buf, 13), CRYPTO_OK);
    CHECK_STR(buf, "Uryyb, Jbeyq");
    CHECK_INT(caesar_encrypt(buf, buf2, sizeof buf2, 13), CRYPTO_OK);
    CHECK_STR(buf2, "Hello, World");

    TEST_SECTION("Decryption");
    CHECK_INT(caesar_decrypt("Dwwdfn dw Gdzq!", buf, sizeof buf, 3), CRYPTO_OK);
    CHECK_STR(buf, "Attack at Dawn!");
    CHECK_INT(caesar_decrypt("abc", buf, sizeof buf, 3), CRYPTO_OK);
    CHECK_STR(buf, "xyz");

    TEST_SECTION("Encrypt -> decrypt round trip (every key)");
    {
        const char *msg = "The Quick Brown Fox Jumps Over The Lazy Dog.";
        int all_ok = 1;
        for (int k = -30; k <= 30; k++) {
            if (caesar_encrypt(msg, buf, sizeof buf, k) != CRYPTO_OK ||
                caesar_decrypt(buf, buf2, sizeof buf2, k) != CRYPTO_OK ||
                strcmp(msg, buf2) != 0) {
                all_ok = 0;
                printf("  round trip failed at key=%d\n", k);
                break;
            }
        }
        CHECK_INT(all_ok, 1);
    }

    TEST_SECTION("Empty string");
    CHECK_INT(caesar_encrypt("", buf, sizeof buf, 5), CRYPTO_OK);
    CHECK_STR(buf, "");

    TEST_SECTION("Error handling");
    CHECK_INT(caesar_encrypt(NULL, buf, sizeof buf, 3), CRYPTO_ERR_NULL);
    CHECK_INT(caesar_encrypt("abc", NULL, 10, 3), CRYPTO_ERR_NULL);
    CHECK_INT(caesar_decrypt(NULL, buf, sizeof buf, 3), CRYPTO_ERR_NULL);
    CHECK_INT(caesar_encrypt("abc", buf, 3, 3), CRYPTO_ERR_BUFFER);  /* no room for NUL */
    CHECK_INT(caesar_encrypt("abc", buf, 4, 3), CRYPTO_OK);          /* exact size */
    CHECK_STR(buf, "def");
    CHECK_INT(caesar_decrypt("abc", buf, 0, 3), CRYPTO_ERR_BUFFER);

    return TEST_SUMMARY();
}
