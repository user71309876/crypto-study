/*
 * xor_cipher.h - XOR cipher, one-time pad, and hex conversion interface
 *
 * From this step on we work with BYTES (uint8_t), not text strings.
 * Encrypted data can contain any byte value, including 0x00, so it can no
 * longer be treated as a NUL-terminated string. That is why every function
 * takes an explicit length.
 *
 * Error handling is optional in every function: the tests only check that
 * correct inputs give correct results. Each function returns CRYPTO_OK.
 */
#ifndef XOR_CIPHER_H
#define XOR_CIPHER_H

#include <stddef.h>
#include <stdint.h>
#include "crypto_common.h"

/*
 * Repeating-key XOR.
 *   out[i] = in[i] ^ key[i % key_len]     for i = 0 .. in_len-1
 *
 * Encryption and decryption are the SAME operation (XOR twice = original).
 *
 *   in, in_len   : input bytes and their count
 *   out          : output buffer. out may be the same pointer as in (in-place).
 *   out_size     : size of out in bytes
 *   key, key_len : key bytes and their count
 *
 * No '\0' is added: out is bytes, not a string.
 */
int xor_cipher(const uint8_t *in, size_t in_len,
               uint8_t *out, size_t out_size,
               const uint8_t *key, size_t key_len);

/*
 * One-time pad (OTP).
 * Same as xor_cipher, but the key is AT LEAST as long as the input,
 * so the key never repeats. Only the first in_len bytes of the key are used.
 */
int otp_encrypt(const uint8_t *in, size_t in_len,
                uint8_t *out, size_t out_size,
                const uint8_t *key, size_t key_len);

/*
 * Bytes -> hex string.
 * Each byte becomes two LOWERCASE hex characters: {0x0b, 0xff} -> "0bff".
 * The result is NUL-terminated (2 * in_len characters + '\0').
 */
int hex_encode(const uint8_t *in, size_t in_len, char *out, size_t out_size);

/*
 * Hex string -> bytes.
 * Accepts both lowercase and uppercase: "0bFF" -> {0x0b, 0xff}.
 *
 *   in       : NUL-terminated hex string (always valid, even length in the tests)
 *   out      : output byte buffer
 *   out_size : size of out in bytes
 *   out_len  : receives the number of bytes written (strlen(in) / 2)
 */
int hex_decode(const char *in, uint8_t *out, size_t out_size, size_t *out_len);

#endif /* XOR_CIPHER_H */
