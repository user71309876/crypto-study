/*
 * test_spn.c - Mini block cipher (16-bit SPN) tests (do not modify)
 */
#include "spn.h"
#include "test.h"

/* Number of 1 bits in x (how many bits differ, when x = a ^ b) */
static int popcount16(uint16_t x)
{
    int n = 0;
    while (x) { n += x & 1; x >>= 1; }
    return n;
}

/* r rounds of "XOR key, substitute, permute", built from YOUR functions */
static uint16_t partial_rounds(uint16_t x, const uint16_t rk[5], int r)
{
    for (int i = 0; i < r; i++)
        x = spn_permute(spn_substitute((uint16_t)(x ^ rk[i])));
    return x;
}

/* The same cipher WITHOUT the S-box: only key XOR and permutation */
static uint16_t no_sbox_cipher(uint16_t x, const uint16_t rk[5])
{
    for (int i = 0; i < 3; i++)
        x = spn_permute((uint16_t)(x ^ rk[i]));
    return (uint16_t)(x ^ rk[3] ^ rk[4]);
}

int main(void)
{
    static const uint8_t S[16] = { 0xE,0x4,0xD,0x1,0x2,0xF,0xB,0x8,0x3,0xA,0x6,0xC,0x5,0x9,0x0,0x7 };
    const uint16_t K[5]    = { 0x1A2B, 0x3C4D, 0x5E6F, 0x7081, 0x92A3 };
    const uint16_t ZERO[5] = { 0, 0, 0, 0, 0 };

    printf("[Step 4] Mini block cipher (16-bit SPN) tests\n");

    /* ------------------------------------------------------------ */
    printf("\n== spn_substitute ==\n");

    TEST_SECTION("Every S-box entry, in every nibble position");
    {
        int all_ok = 1;
        for (int pos = 0; pos < 4 && all_ok; pos++) {
            int shift = 12 - 4 * pos;                       /* nib0 = bits 15..12 */
            for (int v = 0; v < 16; v++) {
                /* other nibbles are 0, and S[0] = 0xE */
                uint16_t in  = (uint16_t)(v << shift);
                uint16_t exp = (uint16_t)((0xEEEE & ~(0xF << shift)) | (S[v] << shift));
                uint16_t got = spn_substitute(in);
                if (got != exp) {
                    printf("  substitute(0x%04X): expected 0x%04X, got 0x%04X\n", in, exp, got);
                    all_ok = 0;
                    break;
                }
            }
        }
        CHECK_INT(all_ok, 1);
    }

    TEST_SECTION("Examples");
    CHECK_INT(spn_substitute(0x0123), 0xE4D1);
    CHECK_INT(spn_substitute(0x4567), 0x2FB8);
    CHECK_INT(spn_substitute(0x89AB), 0x3A6C);
    CHECK_INT(spn_substitute(0xCDEF), 0x5907);
    CHECK_INT(spn_substitute(0xBEEF), 0xC007);

    /* ------------------------------------------------------------ */
    printf("\n== spn_inv_substitute ==\n");

    TEST_SECTION("Examples");
    CHECK_INT(spn_inv_substitute(0xE4D1), 0x0123);
    CHECK_INT(spn_inv_substitute(0x0123), 0xE348);
    CHECK_INT(spn_inv_substitute(0xFFFF), 0x5555);
    CHECK_INT(spn_inv_substitute(0xC007), 0xBEEF);

    TEST_SECTION("Undoes spn_substitute for all 65536 values");
    {
        int all_ok = 1;
        for (uint32_t x = 0; x <= 0xFFFF; x++) {
            if (spn_inv_substitute(spn_substitute((uint16_t)x)) != x) {
                printf("  inv_substitute(substitute(0x%04X)) != 0x%04X\n", (unsigned)x, (unsigned)x);
                all_ok = 0;
                break;
            }
        }
        CHECK_INT(all_ok, 1);
    }

    /* ------------------------------------------------------------ */
    printf("\n== spn_permute ==\n");

    TEST_SECTION("Single bits move to their transposed position");
    CHECK_INT(spn_permute(0x8000), 0x8000);   /* nib0 bit0 -> nib0 bit0 (diagonal stays) */
    CHECK_INT(spn_permute(0x4000), 0x0800);   /* nib0 bit1 -> nib1 bit0 */
    CHECK_INT(spn_permute(0x2000), 0x0080);   /* nib0 bit2 -> nib2 bit0 */
    CHECK_INT(spn_permute(0x1000), 0x0008);   /* nib0 bit3 -> nib3 bit0 */
    CHECK_INT(spn_permute(0x0800), 0x4000);   /* nib1 bit0 -> nib0 bit1 */

    TEST_SECTION("Whole rows become whole columns");
    CHECK_INT(spn_permute(0xF000), 0x8888);   /* row 0 -> column 0 */
    CHECK_INT(spn_permute(0x000F), 0x1111);   /* row 3 -> column 3 */
    CHECK_INT(spn_permute(0x1234), 0x016A);   /* the example in spn.h */
    CHECK_INT(spn_permute(0x8421), 0x8421);   /* the diagonal: nothing moves */

    TEST_SECTION("Applying it twice gives the original (all 65536 values)");
    {
        int all_ok = 1;
        for (uint32_t x = 0; x <= 0xFFFF; x++) {
            if (spn_permute(spn_permute((uint16_t)x)) != x) {
                printf("  permute(permute(0x%04X)) != 0x%04X\n", (unsigned)x, (unsigned)x);
                all_ok = 0;
                break;
            }
        }
        CHECK_INT(all_ok, 1);
    }

    /* ------------------------------------------------------------ */
    printf("\n== spn_encrypt_block ==\n");

    TEST_SECTION("Known values, key K = {1A2B, 3C4D, 5E6F, 7081, 92A3}");
    CHECK_INT(spn_encrypt_block(0x0000, K), 0xDB54);
    CHECK_INT(spn_encrypt_block(0x1234, K), 0x34AB);
    CHECK_INT(spn_encrypt_block(0xABCD, K), 0x1DC5);
    CHECK_INT(spn_encrypt_block(0xFFFF, K), 0x82A6);
    CHECK_INT(spn_encrypt_block(0x26B7, K), 0xF490);

    TEST_SECTION("Known values, all-zero key");
    CHECK_INT(spn_encrypt_block(0x0000, ZERO), 0xE0BB);
    CHECK_INT(spn_encrypt_block(0x1234, ZERO), 0x8A8F);
    CHECK_INT(spn_encrypt_block(0xABCD, ZERO), 0x4105);
    CHECK_INT(spn_encrypt_block(0xFFFF, ZERO), 0x4B00);

    TEST_SECTION("Different plaintexts always give different ciphertexts (all 65536)");
    {
        /* A block cipher with a fixed key is a one-to-one shuffle of all
           2^16 possible blocks. If two plaintexts collided, decryption
           would be impossible. */
        static uint8_t seen[65536];
        int collisions = 0;
        memset(seen, 0, sizeof seen);
        for (uint32_t p = 0; p <= 0xFFFF; p++) {
            uint16_t c = spn_encrypt_block((uint16_t)p, K);
            if (seen[c]) collisions++;
            seen[c] = 1;
        }
        CHECK_INT(collisions, 0);
    }

    /* ------------------------------------------------------------ */
    printf("\n== spn_decrypt_block ==\n");

    TEST_SECTION("Known values");
    CHECK_INT(spn_decrypt_block(0x1DC5, K), 0xABCD);
    CHECK_INT(spn_decrypt_block(0x34AB, K), 0x1234);
    CHECK_INT(spn_decrypt_block(0xF490, K), 0x26B7);
    CHECK_INT(spn_decrypt_block(0x4B00, ZERO), 0xFFFF);

    TEST_SECTION("Encrypt -> decrypt round trip (all 65536 plaintexts)");
    {
        int all_ok = 1;
        for (uint32_t p = 0; p <= 0xFFFF; p++) {
            uint16_t c = spn_encrypt_block((uint16_t)p, K);
            if (spn_decrypt_block(c, K) != p) {
                printf("  decrypt(encrypt(0x%04X)) != 0x%04X\n", (unsigned)p, (unsigned)p);
                all_ok = 0;
                break;
            }
        }
        CHECK_INT(all_ok, 1);
    }

    /* ------------------------------------------------------------ */
    printf("\n== Diffusion: the avalanche effect ==\n");

    TEST_SECTION("Flip 1 plaintext bit: how many output bits change? (ideal = 8 of 16)");
    {
        /* For every plaintext and every one of its 16 bits: flip that bit,
           encrypt both, and count how many output bits differ. */
        const long expected[4] = { 2555904L, 6230016L, 8081232L, 8324104L };
        const char *label[4]   = { "after 1 round  ", "after 2 rounds ",
                                   "after 3 rounds ", "full cipher    " };
        const double n = 65536.0 * 16.0;
        for (int r = 1; r <= 4; r++) {
            long total = 0;
            for (uint32_t p = 0; p <= 0xFFFF; p++) {
                for (int b = 0; b < 16; b++) {
                    uint16_t p2 = (uint16_t)(p ^ (1u << b));
                    uint16_t d = (r < 4)
                        ? (uint16_t)(partial_rounds((uint16_t)p, K, r) ^ partial_rounds(p2, K, r))
                        : (uint16_t)(spn_encrypt_block((uint16_t)p, K) ^ spn_encrypt_block(p2, K));
                    total += popcount16(d);
                }
            }
            printf("    %s average %.2f bits changed\n", label[r - 1], (double)total / n);
            CHECK_INT(total, expected[r - 1]);
        }
    }

    /* ------------------------------------------------------------ */
    printf("\n== Why the S-box matters ==\n");

    TEST_SECTION("ATTACK: without the S-box the cipher is linear");
    {
        /* Without S-boxes, everything is XOR and moving bits around, so
              E(a ^ b) == E(a) ^ E(b) ^ E(0)
           An attacker who has seen E(a), E(b), E(0) can predict E(a ^ b)
           WITHOUT knowing the key. */
        const uint16_t a = 0x1234, b = 0x0F0F;
        uint16_t ea = no_sbox_cipher(a, K), eb = no_sbox_cipher(b, K), e0 = no_sbox_cipher(0, K);
        uint16_t predicted = (uint16_t)(ea ^ eb ^ e0);
        CHECK_INT(no_sbox_cipher((uint16_t)(a ^ b), K), 0x8550);
        CHECK_INT(predicted, 0x8550);                 /* the prediction is exactly right */
    }

    TEST_SECTION("With the S-box the same prediction fails");
    {
        const uint16_t a = 0x1234, b = 0x0F0F;
        uint16_t predicted = (uint16_t)(spn_encrypt_block(a, K) ^ spn_encrypt_block(b, K)
                                        ^ spn_encrypt_block(0, K));
        CHECK_INT(spn_encrypt_block((uint16_t)(a ^ b), K), 0x08C5);
        CHECK_INT(predicted, 0xFFBF);                 /* the prediction is wrong */
    }

    return TEST_SUMMARY();
}
