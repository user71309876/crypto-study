/*
 * xor_cipher.c - Implement this yourself.
 *
 * Run `make test-03` and keep going until there are 0 failures.
 * Suggested order: xor_cipher -> otp_encrypt -> hex_encode -> hex_decode
 */
#include "xor_cipher.h"

int xor_cipher(const uint8_t *in, size_t in_len,
               uint8_t *out, size_t out_size,
               const uint8_t *key, size_t key_len)
{
    (void)in; (void)in_len; (void)out; (void)out_size; (void)key; (void)key_len; /* delete once implemented */

    /* TODO */
    return -99;
}

int otp_encrypt(const uint8_t *in, size_t in_len,
                uint8_t *out, size_t out_size,
                const uint8_t *key, size_t key_len)
{
    (void)in; (void)in_len; (void)out; (void)out_size; (void)key; (void)key_len; /* delete once implemented */

    /* TODO */
    return -99;
}

int hex_encode(const uint8_t *in, size_t in_len, char *out, size_t out_size)
{
    (void)in; (void)in_len; (void)out; (void)out_size; /* delete once implemented */

    /* TODO */
    return -99;
}

int hex_decode(const char *in, uint8_t *out, size_t out_size, size_t *out_len)
{
    (void)in; (void)out; (void)out_size; (void)out_len; /* delete once implemented */

    /* TODO */
    return -99;
}
