/*
 * caesar.h - Caesar cipher interface
 */
#ifndef CAESAR_H
#define CAESAR_H

#include <stddef.h>
#include "crypto_common.h"

/*
 * Shift the plaintext `in` by `key` and write the ciphertext to `out`.
 *
 *   in       : NUL-terminated plaintext string
 *   out      : buffer for the result (same length as in, plus NUL)
 *   out_size : size of the out buffer in bytes
 *   key      : shift amount. Negative, 0, and 26 or more are allowed
 *              (e.g. 29 is the same as 3)
 *
 * Rules:
 *   - Uppercase stays uppercase, lowercase stays lowercase (case preserved)
 *   - Non-letters (spaces, digits, symbols) are left unchanged
 *
 * Returns:
 *   CRYPTO_OK          success
 *   CRYPTO_ERR_NULL    in or out is NULL
 *   CRYPTO_ERR_BUFFER  out_size < strlen(in) + 1
 */
int caesar_encrypt(const char *in, char *out, size_t out_size, int key);

/*
 * Decrypt the ciphertext `in` with `key` and write the plaintext to `out`.
 * Arguments and return values follow the same rules as caesar_encrypt.
 */
int caesar_decrypt(const char *in, char *out, size_t out_size, int key);

#endif /* CAESAR_H */
