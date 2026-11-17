# Step 4: Mini Block Cipher (16-bit SPN)

## What went wrong with Steps 1-3

Every cipher so far changed each character or byte **on its own**:

- Change 1 letter of the plaintext -> exactly 1 letter of the ciphertext changes.
- The same input at the same key position -> always the same output.

That is why frequency analysis works: patterns in the plaintext show through.
The one-time pad avoids this only by using a key as long as the message.

A **block cipher** fixes this with a short key. It takes a whole **block** (here 16 bits,
in AES 128 bits) and mixes it so thoroughly that **changing 1 input bit changes about
half of the output bits**, in a way that looks random.

## Confusion and diffusion

In 1945 Claude Shannon named the two things a good cipher needs:

| Property | Meaning | Done by |
|---|---|---|
| **Confusion** | The relation between key and ciphertext is complex (not simple XOR/shift math) | **S-box** (substitution) |
| **Diffusion** | Each input bit affects many output bits | **Permutation** (moving bits around) |

Neither is enough alone. Repeat both for several **rounds** and they reinforce each other.
This design is called a **Substitution-Permutation Network (SPN)**. AES is an SPN.

## The cipher in this step

The block is 16 bits, seen as 4 nibbles (4 bits each):

```
0x1234 = 0001 0010 0011 0100
         nib0 nib1 nib2 nib3
```

### Part 1: S-box (substitution)

A lookup table that replaces each nibble with another:

```
input   0 1 2 3 4 5 6 7 8 9 A B C D E F
output  E 4 D 1 2 F B 8 3 A 6 C 5 9 0 7
```

`0x0123`: nib 0->E, 1->4, 2->D, 3->1, so the result is `0xE4D1`.

The table is chosen so there is **no simple formula** connecting input and output bits.
That is the "confusion". It must be a one-to-one table (every output appears once),
otherwise you could not undo it. The **inverse S-box** is the same table read backwards:
S[0] = E, so InvS[E] = 0.

### Part 2: Permutation (moving bits)

Write the 4 nibbles as the rows of a 4x4 grid of bits, and **transpose** it:

```
       before                  after
nib0:  0 0 0 1            nib0: 0 0 0 0
nib1:  0 0 1 0     ->     nib1: 0 0 0 1
nib2:  0 0 1 1            nib2: 0 1 1 0
nib3:  0 1 0 0            nib3: 1 0 1 0
       0x1234                   0x016A
```

Why this matters: after it, the 4 bits that came out of **one** S-box go into **four
different** S-boxes in the next round. So a change inside one nibble spreads to all nibbles.
That is the "diffusion". Transposing twice gives the original, so this part is its own inverse.

### Part 3: Key mixing

XOR the block with a 16-bit **round key**. Without this, anyone could run the cipher.
This step uses 5 round keys `rk[0..4]` given directly. (Real ciphers generate round keys
from one key with a **key schedule**. You will build one in AES.)

### Putting it together: 4 rounds

```
plaintext
   |
   XOR rk[0] -> S-box -> permute        round 1
   XOR rk[1] -> S-box -> permute        round 2
   XOR rk[2] -> S-box -> permute        round 3
   XOR rk[3] -> S-box -> XOR rk[4]      round 4 (no permute)
   |
ciphertext
```

The last round skips the permutation: it would only move bits in a fixed, public way,
which an attacker can undo for free. Instead it ends with one more key XOR, so the
attacker cannot peel off the last S-box layer.

### Decryption

Undo every step in **reverse order**, using the inverse of each part:

| Encrypt step | Undone by |
|---|---|
| `XOR rk` | `XOR rk` (same thing, Step 3) |
| S-box | inverse S-box |
| permute | permute (it is its own inverse) |

## What the tests show you

Besides checking your functions, two sections demonstrate the ideas above:

**Avalanche effect.** For every plaintext, flip one bit and count how many ciphertext bits change.
The test measures this after 1, 2, 3 and 4 rounds using your own functions:

```
after 1 round    ~2.4 bits   (only one nibble is affected)
after 2 rounds   ~5.9 bits
after 3 rounds   ~7.7 bits
full cipher      ~7.9 bits   (ideal is 8 = half of 16)
```

This is diffusion in action, and why ciphers need several rounds.

**Why the S-box matters.** The test builds the same cipher **without** S-boxes.
Then it is all XOR and bit moves, which is **linear**:

```
E(a ^ b) = E(a) ^ E(b) ^ E(0)
```

So an attacker can predict ciphertexts without the key. With the S-box this breaks down.
The S-box is the only non-linear part of the cipher; everything else is just XOR and wiring.

## Implementation notes

- Getting nibble n out of x: shift right and mask, like the hex step (`>>`, `& 0xF`).
  nib0 is the highest: bits 15..12.
- The S-box is just an array: `static const uint8_t SBOX[16] = { 0xE, 0x4, ... };`
- For the inverse S-box you can type the table by hand, or build it in a loop from SBOX.
- For the permutation, loop over the 16 bits: read bit k of nibble n, write it to bit n of
  nibble k. Write down which shift amount "bit k of nibble n" is before coding.
- C does arithmetic on `uint16_t` as `int`, so cast back: `(uint16_t)(x ^ rk[0])`.

## Running

```bash
make test-04
```

The tests loop over all 65536 blocks several times; it still takes well under a second.
Suggested order: `spn_substitute` -> `spn_inv_substitute` -> `spn_permute`
-> `spn_encrypt_block` -> `spn_decrypt_block`.

## Pitfalls to think about

1. Why must the S-box be one-to-one? What breaks if two inputs map to the same output?
2. In decryption, which round key do you use first? Why?
3. Why does the test say "different plaintexts always give different ciphertexts"?
   What would it mean for a block cipher if that were not true?
4. Look at the avalanche numbers. Why does 1 round change only ~2.4 bits on average?

## When you are done (optional)

- Print the avalanche result for one plaintext as a bit picture (`0`/`1` per round)
  and watch a single flipped bit spread out.
- Replace the S-box with the identity table (S[i] = i) in a copy of your code. What
  happens to the avalanche numbers? Does `E(a ^ b) = E(a) ^ E(b) ^ E(0)` now hold?
- This exact cipher is the example in Howard Heys' tutorial "A Tutorial on Linear and
  Differential Cryptanalysis", which shows how it can be attacked. Worth reading after AES.
