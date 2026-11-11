/*
 * caesar.c - Caesar cipher implementation
 */
#include <string.h>
#include <ctype.h>
#include "caesar.h"

/* Shift a single character by key */
static char shift_char(char c, int key)
{
    key = key % 26;
    if (key < 0)
        key += 26;

    if (isupper((unsigned char)c))
        return ('A' + (c - 'A' + key) % 26);
    if (islower((unsigned char)c))
        return ('a' + (c - 'a' + key) % 26);
    return c;
}

int caesar_encrypt(const char *in, char *out, size_t out_size, int key)
{
    if (in == NULL || out == NULL)
        return CRYPTO_ERR_NULL;

    size_t len = strlen(in);

    if (out_size < len + 1)
        return CRYPTO_ERR_BUFFER;

    for (size_t i = 0; i < len; i++)
        out[i] = shift_char(in[i], key);

    out[len] = '\0';

    return CRYPTO_OK;
}

int caesar_decrypt(const char *in, char *out, size_t out_size, int key)
{
    return caesar_encrypt(in, out, out_size, -key);
}
