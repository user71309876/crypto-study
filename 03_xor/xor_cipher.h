/*
 * xor_cipher.h - XOR cipher, one-time pad, and hex conversion interface
 *
 * From this step on we work with BYTES (uint8_t), not text strings.
 * Encrypted data can contain any byte value, including 0x00, so it can no
 * longer be treated as a NUL-terminated string. That is why every function
 * takes an explicit length.
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
 * Returns (check in this order):
 *   CRYPTO_ERR_NULL    in, out, or key is NULL
 *   CRYPTO_ERR_KEY     key_len == 0
 *   CRYPTO_ERR_BUFFER  out_size < in_len
 *   CRYPTO_OK          success (no '\0' is added: out is bytes, not a string)
 */
int xor_cipher(const uint8_t *in, size_t in_len,
               uint8_t *out, size_t out_size,
               const uint8_t *key, size_t key_len);

/*
 * One-time pad (OTP).
 * Same as xor_cipher, but the key must be AT LEAST as long as the input,
 * so the key never repeats. Only the first in_len bytes of the key are used.
 *
 * Returns (check in this order):
 *   CRYPTO_ERR_NULL    in, out, or key is NULL
 *   CRYPTO_ERR_KEY     key_len == 0 or key_len < in_len
 *   CRYPTO_ERR_BUFFER  out_size < in_len
 *   CRYPTO_OK          success
 */
int otp_encrypt(const uint8_t *in, size_t in_len,
                uint8_t *out, size_t out_size,
                const uint8_t *key, size_t key_len);

/*
 * Bytes -> hex string.
 * Each byte becomes two LOWERCASE hex characters: {0x0b, 0xff} -> "0bff".
 * The result is NUL-terminated, so out needs 2 * in_len + 1 bytes.
 *
 * Returns (check in this order):
 *   CRYPTO_ERR_NULL    in or out is NULL
 *   CRYPTO_ERR_BUFFER  out_size < 2 * in_len + 1
 *   CRYPTO_OK          success
 */
int hex_encode(const uint8_t *in, size_t in_len, char *out, size_t out_size);

/*
 * Hex string -> bytes.
 * Accepts both lowercase and uppercase: "0bFF" -> {0x0b, 0xff}.
 * On success, the number of bytes written is stored in *out_len.
 *
 *   in       : NUL-terminated hex string
 *   out      : output byte buffer
 *   out_size : size of out in bytes
 *   out_len  : receives the number of bytes written
 *
 * Returns (check in this order):
 *   CRYPTO_ERR_NULL    in, out, or out_len is NULL
 *   CRYPTO_ERR_INPUT   strlen(in) is odd, or contains a non-hex character
 *   CRYPTO_ERR_BUFFER  out_size < strlen(in) / 2
 *   CRYPTO_OK          success
 */
int hex_decode(const char *in, uint8_t *out, size_t out_size, size_t *out_len);

#endif /* XOR_CIPHER_H */
