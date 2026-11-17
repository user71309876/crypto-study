/*
 * vigenere.h - Vigenere cipher interface
 */
#ifndef VIGENERE_H
#define VIGENERE_H

#include <stddef.h>
#include "crypto_common.h"

/*
 * Encrypt the plaintext `in` with the keyword `key` and write it to `out`.
 *
 *   in       : NUL-terminated plaintext string
 *   out      : result buffer (the tests always pass one that is big enough)
 *   key      : keyword made of letters only (case-insensitive: "LEMON" == "lemon")
 *              'A'/'a' shifts by 0, 'B'/'b' by 1, ... 'Z'/'z' by 25
 *
 * Rules:
 *   - Case preserved, non-letters left unchanged
 *   - The key position advances "only when a letter is processed"
 *     (spaces and symbols do not consume the key)
 *   - When the end of the key is reached, start again from the beginning
 *   - out must end with '\0'
 *
 * Returns CRYPTO_OK.
 * (Error handling is optional: the tests only check correct results.)
 */
int vigenere_encrypt(const char *in, char *out, const char *key);

/*
 * Decrypt the ciphertext `in` with `key` and write it to `out`.
 * Same rules as vigenere_encrypt.
 */
int vigenere_decrypt(const char *in, char *out, const char *key);

#endif /* VIGENERE_H */
