/*
 * spn.h - Mini block cipher (16-bit SPN) interface
 *
 * A block cipher encrypts a fixed-size BLOCK at a time. Here the block is
 * 16 bits (one uint16_t). AES (Step 5) has exactly the same structure,
 * just with a 128-bit block and bigger parts.
 *
 * How we look at a 16-bit block: four 4-bit NIBBLES, highest first.
 *
 *     0x1234  =  0001  0010  0011  0100
 *               nib0  nib1  nib2  nib3
 *
 * Round keys: rk[0] .. rk[4], five 16-bit keys (no key schedule in this step).
 */
#ifndef SPN_H
#define SPN_H

#include <stdint.h>

/*
 * S-box: replace each nibble using this table (S[input] = output).
 *
 *   input  0 1 2 3 4 5 6 7 8 9 A B C D E F
 *   output E 4 D 1 2 F B 8 3 A 6 C 5 9 0 7
 *
 * Example: 0x0123 -> 0xE4D1
 */
uint16_t spn_substitute(uint16_t x);

/*
 * Inverse S-box: undo spn_substitute (find i such that S[i] = nibble).
 * spn_inv_substitute(spn_substitute(x)) == x for every x.
 */
uint16_t spn_inv_substitute(uint16_t x);

/*
 * Bit permutation: write the 4 nibbles as the rows of a 4x4 bit grid and
 * TRANSPOSE it (rows become columns).
 *
 *   0x1234:  nib0 = 0 0 0 1         result:  nib0 = 0 0 0 0   (column 0)
 *            nib1 = 0 0 1 0                  nib1 = 0 0 0 1   (column 1)
 *            nib2 = 0 0 1 1    ->            nib2 = 0 1 1 0   (column 2)
 *            nib3 = 0 1 0 0                  nib3 = 1 0 1 0   (column 3)
 *                                    = 0x016A
 *
 * Bit k of nibble n (k = 0 is the nibble's leftmost bit) moves to
 * bit n of nibble k. Applying it twice gives the original.
 */
uint16_t spn_permute(uint16_t x);

/*
 * Encrypt one 16-bit block with 4 rounds:
 *
 *   rounds 1-3:  x ^= rk[r];  x = substitute(x);  x = permute(x);
 *   round 4:     x ^= rk[3];  x = substitute(x);  x ^= rk[4];
 *
 * (The last round has no permutation; it would not add any security.)
 */
uint16_t spn_encrypt_block(uint16_t plaintext, const uint16_t rk[5]);

/*
 * Decrypt one block: every step of spn_encrypt_block undone in REVERSE order.
 * spn_decrypt_block(spn_encrypt_block(p, rk), rk) == p for every p.
 */
uint16_t spn_decrypt_block(uint16_t ciphertext, const uint16_t rk[5]);

#endif /* SPN_H */
