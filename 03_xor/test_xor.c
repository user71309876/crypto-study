/*
 * test_xor.c - XOR cipher / one-time pad / hex tests (do not modify)
 */
#include "xor_cipher.h"
#include "test.h"

int main(void)
{
    uint8_t buf[256];
    uint8_t buf2[256];
    char    hex[600];   /* 256 bytes -> 512 hex chars + NUL */
    size_t  n;

    /* Fill with a known junk value so a function that writes nothing can't pass by luck */
    memset(buf, 0xCC, sizeof buf);
    memset(buf2, 0xCC, sizeof buf2);
    memset(hex, 'Z', sizeof hex - 1);
    hex[sizeof hex - 1] = '\0';

    printf("[Step 3] XOR cipher / one-time pad / hex tests\n");

    /* ------------------------------------------------------------ */
    printf("\n== xor_cipher ==\n");

    TEST_SECTION("Single byte: the XOR truth table");
    {
        const uint8_t in[]  = { 0x0F };            /* 0000 1111 */
        const uint8_t key[] = { 0x55 };            /* 0101 0101 */
        const uint8_t exp[] = { 0x5A };            /* 0101 1010 */
        CHECK_INT(xor_cipher(in, 1, buf, sizeof buf, key, 1), CRYPTO_OK);
        CHECK_BYTES(buf, exp, 1);
    }

    TEST_SECTION("Key 0x00 changes nothing, key 0xFF flips every bit");
    {
        const uint8_t in[]    = { 0x00, 0x12, 0xAB, 0xFF };
        const uint8_t zero[]  = { 0x00 };
        const uint8_t ff[]    = { 0xFF };
        const uint8_t flipped[] = { 0xFF, 0xED, 0x54, 0x00 };
        CHECK_INT(xor_cipher(in, 4, buf, sizeof buf, zero, 1), CRYPTO_OK);
        CHECK_BYTES(buf, in, 4);
        CHECK_INT(xor_cipher(in, 4, buf, sizeof buf, ff, 1), CRYPTO_OK);
        CHECK_BYTES(buf, flipped, 4);
    }

    TEST_SECTION("XOR with itself gives all zeros");
    {
        const uint8_t in[]    = { 'K', 'E', 'Y' };
        const uint8_t zeros[] = { 0, 0, 0 };
        CHECK_INT(xor_cipher(in, 3, buf, sizeof buf, in, 3), CRYPTO_OK);
        CHECK_BYTES(buf, zeros, 3);
    }

    TEST_SECTION("Repeating key (known test vector, key \"ICE\")");
    {
        const char *plain =
            "Burning 'em, if you ain't quick and nimble\n"
            "I go crazy when I hear a cymbal";
        const uint8_t expected[] = {
            0x0b,0x36,0x37,0x27,0x2a,0x2b,0x2e,0x63,0x62,0x2c,0x2e,0x69,0x69,0x2a,0x23,0x69,
            0x3a,0x2a,0x3c,0x63,0x24,0x20,0x2d,0x62,0x3d,0x63,0x34,0x3c,0x2a,0x26,0x22,0x63,
            0x24,0x27,0x27,0x65,0x27,0x2a,0x28,0x2b,0x2f,0x20,0x43,0x0a,0x65,0x2e,0x2c,0x65,
            0x2a,0x31,0x24,0x33,0x3a,0x65,0x3e,0x2b,0x20,0x27,0x63,0x0c,0x69,0x2b,0x20,0x28,
            0x31,0x65,0x28,0x63,0x26,0x30,0x2e,0x27,0x28,0x2f
        };
        CHECK_INT(xor_cipher((const uint8_t *)plain, strlen(plain), buf, sizeof buf,
                             (const uint8_t *)"ICE", 3), CRYPTO_OK);
        CHECK_BYTES(buf, expected, sizeof expected);
    }

    TEST_SECTION("Encrypting twice gives the original (encrypt = decrypt)");
    {
        const uint8_t in[]  = { 'H', 'i', 0x00, 0x7F, 0x80, 0xFF, '!' };   /* 0x00 in the middle */
        const uint8_t key[] = { 0xDE, 0xAD, 0xBE, 0xEF };
        CHECK_INT(xor_cipher(in, sizeof in, buf, sizeof buf, key, sizeof key), CRYPTO_OK);
        CHECK_INT(xor_cipher(buf, sizeof in, buf2, sizeof buf2, key, sizeof key), CRYPTO_OK);
        CHECK_BYTES(buf2, in, sizeof in);
    }

    TEST_SECTION("In-place: out is the same buffer as in");
    {
        uint8_t data[]      = { 0x10, 0x20, 0x30, 0x40, 0x50 };
        const uint8_t key[] = { 0x01, 0x02 };
        const uint8_t exp[] = { 0x11, 0x22, 0x31, 0x42, 0x51 };
        CHECK_INT(xor_cipher(data, sizeof data, data, sizeof data, key, sizeof key), CRYPTO_OK);
        CHECK_BYTES(data, exp, sizeof exp);
    }

    TEST_SECTION("Empty input");
    CHECK_INT(xor_cipher(buf2, 0, buf, sizeof buf, (const uint8_t *)"K", 1), CRYPTO_OK);

    TEST_SECTION("Errors");
    CHECK_INT(xor_cipher(NULL, 1, buf, sizeof buf, (const uint8_t *)"K", 1), CRYPTO_ERR_NULL);
    CHECK_INT(xor_cipher(buf2, 1, NULL, 1, (const uint8_t *)"K", 1), CRYPTO_ERR_NULL);
    CHECK_INT(xor_cipher(buf2, 1, buf, sizeof buf, NULL, 1), CRYPTO_ERR_NULL);
    CHECK_INT(xor_cipher(buf2, 1, buf, sizeof buf, (const uint8_t *)"K", 0), CRYPTO_ERR_KEY);
    CHECK_INT(xor_cipher(buf2, 5, buf, 4, (const uint8_t *)"K", 1), CRYPTO_ERR_BUFFER);
    CHECK_INT(xor_cipher(buf2, 5, buf, 5, (const uint8_t *)"K", 1), CRYPTO_OK);   /* exact size, no NUL */
    CHECK_INT(xor_cipher(buf2, 5, buf, 4, (const uint8_t *)"K", 0), CRYPTO_ERR_KEY); /* key before buffer */

    /* ------------------------------------------------------------ */
    printf("\n== otp_encrypt ==\n");

    TEST_SECTION("Key as long as the message");
    {
        const uint8_t in[]  = { 'O', 'T', 'P' };
        const uint8_t key[] = { 0x01, 0x02, 0x03 };
        const uint8_t exp[] = { 'O' ^ 0x01, 'T' ^ 0x02, 'P' ^ 0x03 };
        CHECK_INT(otp_encrypt(in, 3, buf, sizeof buf, key, 3), CRYPTO_OK);
        CHECK_BYTES(buf, exp, 3);
    }

    TEST_SECTION("Longer key is fine (only the first in_len bytes are used)");
    {
        const uint8_t in[]  = { 0xAA, 0xBB };
        const uint8_t key[] = { 0x0F, 0xF0, 0x99, 0x99 };
        const uint8_t exp[] = { 0xA5, 0x4B };
        CHECK_INT(otp_encrypt(in, 2, buf, sizeof buf, key, 4), CRYPTO_OK);
        CHECK_BYTES(buf, exp, 2);
    }

    TEST_SECTION("Key shorter than the message is rejected");
    {
        const uint8_t in[]  = { 1, 2, 3, 4 };
        const uint8_t key[] = { 9, 9, 9 };
        CHECK_INT(otp_encrypt(in, 4, buf, sizeof buf, key, 3), CRYPTO_ERR_KEY);
        CHECK_INT(otp_encrypt(in, 4, buf, 1, key, 3), CRYPTO_ERR_KEY);    /* key before buffer */
        CHECK_INT(otp_encrypt(in, 4, buf, 3, key, 4), CRYPTO_ERR_BUFFER);
        CHECK_INT(otp_encrypt(NULL, 4, buf, sizeof buf, key, 3), CRYPTO_ERR_NULL);
    }

    TEST_SECTION("ATTACK: reusing a one-time pad key leaks p1 ^ p2");
    {
        /* The attacker sees only c1 and c2, never the key. */
        const char *p1 = "ATTACK AT DAWN";
        const char *p2 = "RETREAT AT TEN";
        const uint8_t key[14] = { 0x3c,0x91,0x5e,0x07,0xd2,0x48,0xa3,0x6f,
                                  0x10,0xbb,0x29,0xe4,0x75,0xc8 };
        uint8_t c1[14], c2[14], c1_xor_c2[14], p1_xor_p2[14], recovered[14];
        memset(c1_xor_c2, 0xAA, sizeof c1_xor_c2);   /* different junk so they can't match by luck */
        memset(p1_xor_p2, 0x55, sizeof p1_xor_p2);
        memset(recovered, 0xCC, sizeof recovered);

        CHECK_INT(otp_encrypt((const uint8_t *)p1, 14, c1, sizeof c1, key, 14), CRYPTO_OK);
        CHECK_INT(otp_encrypt((const uint8_t *)p2, 14, c2, sizeof c2, key, 14), CRYPTO_OK);

        /* c1 ^ c2 = (p1 ^ k) ^ (p2 ^ k) = p1 ^ p2 : the key cancels out! */
        CHECK_INT(xor_cipher(c1, 14, c1_xor_c2, sizeof c1_xor_c2, c2, 14), CRYPTO_OK);
        CHECK_INT(xor_cipher((const uint8_t *)p1, 14, p1_xor_p2, sizeof p1_xor_p2,
                             (const uint8_t *)p2, 14), CRYPTO_OK);
        CHECK_BYTES(c1_xor_c2, p1_xor_p2, 14);

        /* If the attacker guesses p1, they get p2 without ever knowing the key. */
        CHECK_INT(xor_cipher(c1_xor_c2, 14, recovered, sizeof recovered,
                             (const uint8_t *)p1, 14), CRYPTO_OK);
        CHECK_BYTES(recovered, p2, 14);
    }

    /* ------------------------------------------------------------ */
    printf("\n== hex_encode ==\n");

    TEST_SECTION("Basic encoding (lowercase)");
    {
        const uint8_t in[] = { 0x00, 0x0b, 0x7f, 0x80, 0xab, 0xff };
        CHECK_INT(hex_encode(in, sizeof in, hex, sizeof hex), CRYPTO_OK);
        CHECK_STR(hex, "000b7f80abff");
        CHECK_INT(hex_encode((const uint8_t *)"Hi!", 3, hex, sizeof hex), CRYPTO_OK);
        CHECK_STR(hex, "486921");
    }

    TEST_SECTION("Empty input gives empty string");
    CHECK_INT(hex_encode(buf2, 0, hex, sizeof hex), CRYPTO_OK);
    CHECK_STR(hex, "");

    TEST_SECTION("Errors");
    CHECK_INT(hex_encode(NULL, 1, hex, sizeof hex), CRYPTO_ERR_NULL);
    CHECK_INT(hex_encode(buf2, 1, NULL, 3), CRYPTO_ERR_NULL);
    CHECK_INT(hex_encode((const uint8_t *)"ab", 2, hex, 4), CRYPTO_ERR_BUFFER);  /* needs 5 */
    CHECK_INT(hex_encode((const uint8_t *)"ab", 2, hex, 5), CRYPTO_OK);
    CHECK_STR(hex, "6162");

    /* ------------------------------------------------------------ */
    printf("\n== hex_decode ==\n");

    TEST_SECTION("Basic decoding, lowercase and uppercase");
    {
        const uint8_t exp[] = { 0x00, 0x0b, 0x7f, 0x80, 0xab, 0xff };
        n = 999;
        CHECK_INT(hex_decode("000b7f80abff", buf, sizeof buf, &n), CRYPTO_OK);
        CHECK_INT(n, 6);
        CHECK_BYTES(buf, exp, 6);
        n = 999;
        CHECK_INT(hex_decode("000B7F80ABFF", buf, sizeof buf, &n), CRYPTO_OK);
        CHECK_INT(n, 6);
        CHECK_BYTES(buf, exp, 6);
        CHECK_INT(hex_decode("aBcD", buf, sizeof buf, &n), CRYPTO_OK);
        CHECK_INT(n, 2);
        CHECK_BYTES(buf, "\xab\xcd", 2);
    }

    TEST_SECTION("Empty string gives 0 bytes");
    n = 999;
    CHECK_INT(hex_decode("", buf, sizeof buf, &n), CRYPTO_OK);
    CHECK_INT(n, 0);

    TEST_SECTION("Round trip: bytes -> hex -> bytes");
    {
        uint8_t all[256];
        for (int i = 0; i < 256; i++) all[i] = (uint8_t)i;
        CHECK_INT(hex_encode(all, 256, hex, sizeof hex), CRYPTO_OK);
        CHECK_INT(hex_decode(hex, buf, sizeof buf, &n), CRYPTO_OK);
        CHECK_INT(n, 256);
        CHECK_BYTES(buf, all, 256);
    }

    TEST_SECTION("Errors");
    CHECK_INT(hex_decode(NULL, buf, sizeof buf, &n), CRYPTO_ERR_NULL);
    CHECK_INT(hex_decode("00", NULL, 1, &n), CRYPTO_ERR_NULL);
    CHECK_INT(hex_decode("00", buf, sizeof buf, NULL), CRYPTO_ERR_NULL);
    CHECK_INT(hex_decode("abc", buf, sizeof buf, &n), CRYPTO_ERR_INPUT);   /* odd length */
    CHECK_INT(hex_decode("0g", buf, sizeof buf, &n), CRYPTO_ERR_INPUT);    /* 'g' is not hex */
    CHECK_INT(hex_decode("12 4", buf, sizeof buf, &n), CRYPTO_ERR_INPUT);  /* space */
    CHECK_INT(hex_decode("0x12", buf, sizeof buf, &n), CRYPTO_ERR_INPUT);  /* no "0x" prefix */
    CHECK_INT(hex_decode("\xEA\xB0", buf, sizeof buf, &n), CRYPTO_ERR_INPUT); /* non-ASCII bytes */
    CHECK_INT(hex_decode("aabbcc", buf, 2, &n), CRYPTO_ERR_BUFFER);        /* needs 3 */
    CHECK_INT(hex_decode("aabbc", buf, 1, &n), CRYPTO_ERR_INPUT);          /* input before buffer */
    CHECK_INT(hex_decode("aabbcc", buf, 3, &n), CRYPTO_OK);                /* exact size */
    CHECK_INT(n, 3);

    return TEST_SUMMARY();
}
