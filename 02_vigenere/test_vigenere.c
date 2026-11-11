/*
 * test_vigenere.c - Vigenere cipher tests (do not modify)
 */
#include "vigenere.h"
#include "test.h"

int main(void)
{
    char buf[256];
    char buf2[256];

    printf("[Step 2] Vigenere cipher tests\n");

    TEST_SECTION("Textbook example: ATTACKATDAWN / LEMON");
    CHECK_INT(vigenere_encrypt("ATTACKATDAWN", buf, sizeof buf, "LEMON"), CRYPTO_OK);
    CHECK_STR(buf, "LXFOPVEFRNHR");

    TEST_SECTION("Key of length 1 = Caesar cipher");
    CHECK_INT(vigenere_encrypt("XYZ", buf, sizeof buf, "D"), CRYPTO_OK); /* D = 3 */
    CHECK_STR(buf, "ABC");
    CHECK_INT(vigenere_encrypt("Hello", buf, sizeof buf, "A"), CRYPTO_OK); /* A = 0 */
    CHECK_STR(buf, "Hello");

    TEST_SECTION("Key is case-insensitive");
    CHECK_INT(vigenere_encrypt("ATTACKATDAWN", buf, sizeof buf, "lemon"), CRYPTO_OK);
    CHECK_STR(buf, "LXFOPVEFRNHR");
    CHECK_INT(vigenere_encrypt("ATTACKATDAWN", buf, sizeof buf, "LeMoN"), CRYPTO_OK);
    CHECK_STR(buf, "LXFOPVEFRNHR");

    TEST_SECTION("Plaintext case preserved");
    CHECK_INT(vigenere_encrypt("attackatdawn", buf, sizeof buf, "LEMON"), CRYPTO_OK);
    CHECK_STR(buf, "lxfopvefrnhr");

    TEST_SECTION("Spaces/symbols unchanged and do not consume the key");
    CHECK_INT(vigenere_encrypt("Attack at Dawn!", buf, sizeof buf, "LEMON"), CRYPTO_OK);
    CHECK_STR(buf, "Lxfopv ef Rnhr!");
    CHECK_INT(vigenere_encrypt("A-B-C", buf, sizeof buf, "BC"), CRYPTO_OK);
    CHECK_STR(buf, "B-D-D");

    TEST_SECTION("Decryption");
    CHECK_INT(vigenere_decrypt("LXFOPVEFRNHR", buf, sizeof buf, "LEMON"), CRYPTO_OK);
    CHECK_STR(buf, "ATTACKATDAWN");
    CHECK_INT(vigenere_decrypt("Lxfopv ef Rnhr!", buf, sizeof buf, "lemon"), CRYPTO_OK);
    CHECK_STR(buf, "Attack at Dawn!");

    TEST_SECTION("Encrypt -> decrypt round trip");
    {
        const char *msg = "The Quick Brown Fox Jumps Over The Lazy Dog, 1234!";
        const char *keys[] = {"A", "Z", "KEY", "Crypto", "VERYLONGKEYWORDTHATISLONGERTHANTHEMESSAGEITSELF"};
        int all_ok = 1;
        for (size_t i = 0; i < sizeof keys / sizeof keys[0]; i++)
        {
            if (vigenere_encrypt(msg, buf, sizeof buf, keys[i]) != CRYPTO_OK ||
                vigenere_decrypt(buf, buf2, sizeof buf2, keys[i]) != CRYPTO_OK ||
                strcmp(msg, buf2) != 0)
            {
                all_ok = 0;
                printf("  round trip failed at key=\"%s\"\n", keys[i]);
            }
        }
        CHECK_INT(all_ok, 1);
    }

    TEST_SECTION("Empty string");
    CHECK_INT(vigenere_encrypt("", buf, sizeof buf, "KEY"), CRYPTO_OK);
    CHECK_STR(buf, "");

    TEST_SECTION("Error handling: NULL");
    CHECK_INT(vigenere_encrypt(NULL, buf, sizeof buf, "KEY"), CRYPTO_ERR_NULL);
    CHECK_INT(vigenere_encrypt("abc", NULL, 10, "KEY"), CRYPTO_ERR_NULL);
    CHECK_INT(vigenere_encrypt("abc", buf, sizeof buf, NULL), CRYPTO_ERR_NULL);
    CHECK_INT(vigenere_decrypt("abc", buf, sizeof buf, NULL), CRYPTO_ERR_NULL);

    TEST_SECTION("Error handling: invalid key");
    CHECK_INT(vigenere_encrypt("abc", buf, sizeof buf, ""), CRYPTO_ERR_KEY);
    CHECK_INT(vigenere_encrypt("abc", buf, sizeof buf, "KEY1"), CRYPTO_ERR_KEY);
    CHECK_INT(vigenere_encrypt("abc", buf, sizeof buf, "MY KEY"), CRYPTO_ERR_KEY);
    CHECK_INT(vigenere_decrypt("abc", buf, sizeof buf, "-"), CRYPTO_ERR_KEY);

    TEST_SECTION("Error handling: buffer size");
    CHECK_INT(vigenere_encrypt("abc", buf, 3, "KEY"), CRYPTO_ERR_BUFFER);
    CHECK_INT(vigenere_encrypt("abc", buf, 4, "B"), CRYPTO_OK);
    CHECK_STR(buf, "bcd");

    TEST_SECTION("Error priority: key error comes before buffer error");
    CHECK_INT(vigenere_encrypt("abc", buf, 1, ""), CRYPTO_ERR_KEY);
    CHECK_INT(vigenere_encrypt("abc", buf, 1, "KEY1"), CRYPTO_ERR_KEY); /* bad letter, tiny buffer */
    CHECK_INT(vigenere_encrypt("abc", buf, 0, "MY KEY"), CRYPTO_ERR_KEY);
    CHECK_INT(vigenere_decrypt("abc", buf, 1, "KEY1"), CRYPTO_ERR_KEY);

    TEST_SECTION("Error priority: NULL comes before key and buffer errors");
    CHECK_INT(vigenere_encrypt(NULL, buf, 1, ""), CRYPTO_ERR_NULL);
    CHECK_INT(vigenere_encrypt("abc", NULL, 1, "KEY1"), CRYPTO_ERR_NULL);

    TEST_SECTION("Non-ASCII bytes (e.g. UTF-8 Korean) are unchanged and do not consume the key");
    /* "\xEA\xB0\x80" is the UTF-8 encoding of one Korean character (3 bytes) */
    CHECK_INT(vigenere_encrypt("\xEA\xB0\x80 ab", buf, sizeof buf, "BC"), CRYPTO_OK);
    CHECK_STR(buf, "\xEA\xB0\x80 bd");
    CHECK_INT(vigenere_encrypt("caf\xC3\xA9 x", buf, sizeof buf, "B"), CRYPTO_OK);
    CHECK_STR(buf, "dbg\xC3\xA9 y");
    CHECK_INT(vigenere_decrypt("\xEA\xB0\x80 bd", buf, sizeof buf, "BC"), CRYPTO_OK);
    CHECK_STR(buf, "\xEA\xB0\x80 ab");

    return TEST_SUMMARY();
}