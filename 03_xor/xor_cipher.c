/*
 * xor_cipher.c - Implement this yourself.
 *
 * Run `make test-03` and keep going until there are 0 failures.
 * Suggested order: xor_cipher -> otp_encrypt -> hex_encode -> hex_decode
 */
#include "xor_cipher.h"
#include <string.h>

const char digits[] = "0123456789abcdef";

int xor_cipher(const uint8_t *in, size_t in_len,
               uint8_t *out,
               const uint8_t *key, size_t key_len)
{
    for (size_t i = 0; i < in_len; i++)
    {
        out[i] = in[i] ^ key[i % key_len];
    }
    return CRYPTO_OK;
}

int otp_encrypt(const uint8_t *in, size_t in_len,
                uint8_t *out,
                const uint8_t *key, size_t key_len)
{
    return xor_cipher(in, in_len, out, key, key_len);
}

int hex_encode(const uint8_t *in, size_t in_len, char *out)
{
    for (size_t i = 0; i < in_len; i++)
    {
        out[i * 2] = digits[(int)(in[i] >> 4)];
        out[i * 2 + 1] = digits[(int)(in[i] & 0x0F)];
    }
    out[in_len * 2] = '\0';
    return CRYPTO_OK;
}

static int hex_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
}

int hex_decode(const char *in, uint8_t *out, size_t *out_len)
{
    size_t in_len = strlen(in);

    for (size_t i = 0; i < in_len / 2; i++)
    {
        out[i] = (uint8_t)(hex_value(in[i * 2]) << 4 | hex_value(in[i * 2 + 1]));
    }
    *out_len = in_len / 2;
    return CRYPTO_OK;
}
