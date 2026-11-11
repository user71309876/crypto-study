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
 *   out      : result buffer
 *   out_size : size of the out buffer in bytes
 *   key      : keyword made of letters only (case-insensitive: "LEMON" == "lemon")
 *              'A'/'a' shifts by 0, 'B'/'b' by 1, ... 'Z'/'z' by 25
 *
 * Rules:
 *   - Case preserved, non-letters left unchanged
 *   - The key position advances "only when a letter is processed"
 *     (spaces and symbols do not consume the key)
 *   - When the end of the key is reached, start again from the beginning
 *
 * Returns:
 *   CRYPTO_OK          success
 *   CRYPTO_ERR_NULL    one of in, out, key is NULL
 *   CRYPTO_ERR_KEY     key is empty or contains a non-letter
 *   CRYPTO_ERR_BUFFER  out_size < strlen(in) + 1
 *   (If several errors apply, return the first one in the order above)
 */
int vigenere_encrypt(const char *in, char *out, size_t out_size, const char *key);

/*
 * Decrypt the ciphertext `in` with `key` and write it to `out`.
 * Same rules as vigenere_encrypt.
 */
int vigenere_decrypt(const char *in, char *out, size_t out_size, const char *key);

#endif /* VIGENERE_H */
