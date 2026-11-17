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
 *   out      : buffer for the result (the tests always pass one that is big enough)
 *   key      : shift amount. Negative, 0, and 26 or more are allowed
 *              (e.g. 29 is the same as 3)
 *
 * Rules:
 *   - Uppercase stays uppercase, lowercase stays lowercase (case preserved)
 *   - Non-letters (spaces, digits, symbols) are left unchanged
 *   - out must end with '\0'
 *
 * Returns CRYPTO_OK.
 * (Error handling is optional: the tests only check correct results.)
 */
int caesar_encrypt(const char *in, char *out, int key);

/*
 * Decrypt the ciphertext `in` with `key` and write the plaintext to `out`.
 * Same arguments and rules as caesar_encrypt.
 */
int caesar_decrypt(const char *in, char *out, int key);

#endif /* CAESAR_H */
