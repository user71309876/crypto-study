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
    CHECK_INT(vigenere_encrypt("ATTACKATDAWN", buf, "LEMON"), CRYPTO_OK);
    CHECK_STR(buf, "LXFOPVEFRNHR");

    TEST_SECTION("Key of length 1 = Caesar cipher");
    CHECK_INT(vigenere_encrypt("XYZ", buf, "D"), CRYPTO_OK);   /* D = 3 */
    CHECK_STR(buf, "ABC");
    CHECK_INT(vigenere_encrypt("Hello", buf, "A"), CRYPTO_OK); /* A = 0 */
    CHECK_STR(buf, "Hello");

    TEST_SECTION("Key is case-insensitive");
    CHECK_INT(vigenere_encrypt("ATTACKATDAWN", buf, "lemon"), CRYPTO_OK);
    CHECK_STR(buf, "LXFOPVEFRNHR");
    CHECK_INT(vigenere_encrypt("ATTACKATDAWN", buf, "LeMoN"), CRYPTO_OK);
    CHECK_STR(buf, "LXFOPVEFRNHR");

    TEST_SECTION("Plaintext case preserved");
    CHECK_INT(vigenere_encrypt("attackatdawn", buf, "LEMON"), CRYPTO_OK);
    CHECK_STR(buf, "lxfopvefrnhr");

    TEST_SECTION("Spaces/symbols unchanged and do not consume the key");
    CHECK_INT(vigenere_encrypt("Attack at Dawn!", buf, "LEMON"), CRYPTO_OK);
    CHECK_STR(buf, "Lxfopv ef Rnhr!");
    CHECK_INT(vigenere_encrypt("A-B-C", buf, "BC"), CRYPTO_OK);
    CHECK_STR(buf, "B-D-D");

    TEST_SECTION("Decryption");
    CHECK_INT(vigenere_decrypt("LXFOPVEFRNHR", buf, "LEMON"), CRYPTO_OK);
    CHECK_STR(buf, "ATTACKATDAWN");
    CHECK_INT(vigenere_decrypt("Lxfopv ef Rnhr!", buf, "lemon"), CRYPTO_OK);
    CHECK_STR(buf, "Attack at Dawn!");

    TEST_SECTION("Encrypt -> decrypt round trip");
    {
        const char *msg  = "The Quick Brown Fox Jumps Over The Lazy Dog, 1234!";
        const char *keys[] = { "A", "Z", "KEY", "Crypto", "VERYLONGKEYWORDTHATISLONGERTHANTHEMESSAGEITSELF" };
        int all_ok = 1;
        for (size_t i = 0; i < sizeof keys / sizeof keys[0]; i++) {
            if (vigenere_encrypt(msg, buf, keys[i]) != CRYPTO_OK ||
                vigenere_decrypt(buf, buf2, keys[i]) != CRYPTO_OK ||
                strcmp(msg, buf2) != 0) {
                all_ok = 0;
                printf("  round trip failed at key=\"%s\"\n", keys[i]);
            }
        }
        CHECK_INT(all_ok, 1);
    }

    TEST_SECTION("Empty string");
    CHECK_INT(vigenere_encrypt("", buf, "KEY"), CRYPTO_OK);
    CHECK_STR(buf, "");

    TEST_SECTION("Non-ASCII bytes (e.g. UTF-8 Korean) are unchanged and do not consume the key");
    /* "\xEA\xB0\x80" is the UTF-8 encoding of one Korean character (3 bytes) */
    CHECK_INT(vigenere_encrypt("\xEA\xB0\x80 ab", buf, "BC"), CRYPTO_OK);
    CHECK_STR(buf, "\xEA\xB0\x80 bd");
    CHECK_INT(vigenere_encrypt("caf\xC3\xA9 x", buf, "B"), CRYPTO_OK);
    CHECK_STR(buf, "dbg\xC3\xA9 y");
    CHECK_INT(vigenere_decrypt("\xEA\xB0\x80 bd", buf, "BC"), CRYPTO_OK);
    CHECK_STR(buf, "\xEA\xB0\x80 ab");

    return TEST_SUMMARY();
}
