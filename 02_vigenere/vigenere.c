/*
 * vigenere.c - Implement this yourself.
 *
 * Run `make test-02` and keep going until there are 0 failures.
 */
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "vigenere.h"

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

static int vigenere_run(const char *in, char *out, size_t out_size, const char *key, bool decrypt)
{
    if (in == NULL || out == NULL || key == NULL)
        return CRYPTO_ERR_NULL;

    size_t key_len = strlen(key);
    if (key_len == 0)
        return CRYPTO_ERR_KEY;
    for (size_t i = 0; i < key_len; i++)
    {
        if (!isalpha((unsigned char)key[i]))
            return CRYPTO_ERR_KEY;
    }

    size_t in_len = strlen(in);
    if (out_size < in_len + 1)
        return CRYPTO_ERR_BUFFER;

    for (size_t i = 0, j = 0; i < in_len; i++, j++)
    {
        if (isalpha((unsigned char)in[i]))
        {
            int move = toupper((unsigned char)key[j % key_len]) - 'A';
            if (decrypt)
                move = (26 - move) % 26;
            out[i] = shift_char(in[i], move);
        }
        else
        {
            out[i] = in[i];
            j--;
        }
    }
    out[in_len] = '\0';

    return CRYPTO_OK;
}

int vigenere_encrypt(const char *in, char *out, size_t out_size, const char *key)
{
    return vigenere_run(in, out, out_size, key, false);
}

int vigenere_decrypt(const char *in, char *out, size_t out_size, const char *key)
{
    return vigenere_run(in, out, out_size, key, true);
}
