# Step 3: XOR Cipher and the One-Time Pad

## From letters to bytes

Steps 1 and 2 only worked on the 26 letters A-Z. Real data (files, network packets, images)
is made of **bytes**, each 0 to 255. From this step on, every cipher works on bytes.

This changes how we handle data in C:

- Encrypted bytes can be **0x00**. In C, 0x00 is `'\0'`, the end of a string.
  So `strlen` and `printf("%s")` would stop in the middle of the data.
- That is why every function now takes an explicit **length** (`in_len`), uses `uint8_t`
  (an unsigned 8-bit byte, 0..255), and does **not** add a `'\0'` to the output.
- To look at bytes as text we use **hex** (hexadecimal): each byte becomes 2 characters,
  e.g. 0x0B -> `"0b"`. You will build this too.

## What XOR is

XOR (exclusive OR, `^` in C) compares two bits: the result is 1 when they are **different**.

```
a  b  | a ^ b
0  0  |   0
0  1  |   1
1  0  |   1
1  1  |   0
```

On a byte, it is applied to each of the 8 bits separately:

```
  0000 1111   (0x0F)  plaintext
^ 0101 0101   (0x55)  key
-----------
  0101 1010   (0x5A)  ciphertext
```

Think of the key bit as a switch: **key bit 1 = flip that bit, key bit 0 = leave it**.

### The properties that make it a cipher

| Property | Meaning |
|---|---|
| `x ^ 0 = x` | A key of 0x00 changes nothing |
| `x ^ x = 0` | Anything XORed with itself disappears |
| `(x ^ k) ^ k = x` | **Applying the same key twice gives back the original** |

The third line is why XOR is a cipher: `C = P ^ K` and `P = C ^ K`.
**Encryption and decryption are the same function.** Compare that with Steps 1 and 2,
where decryption needed the opposite shift.

## Repeating-key XOR (`xor_cipher`)

If the key is shorter than the data, repeat it, exactly like the Vigenere key:

```
out[i] = in[i] ^ key[i % key_len]
```

```
Plain   B    u    r    n    i    n    g
Key     I    C    E    I    C    E    I
Cipher  0x0b 0x36 0x37 0x27 0x2a 0x2b 0x2e
```

This is basically the Vigenere cipher on bytes, so it has the same weakness: the key repeats,
so it can be broken with the same key-length + frequency analysis.

## The one-time pad (`otp_encrypt`)

What if the key:
1. is **as long as the message** (never repeats),
2. is **truly random**, and
3. is **used only once**?

Then the ciphertext gives **no information at all** about the plaintext. For any ciphertext,
every plaintext of the same length is equally possible, because there is a key that maps to each
one. This is called **perfect secrecy**, proven by Claude Shannon in 1949. It is the only cipher
that is mathematically unbreakable, no matter how much computing power the attacker has.

`otp_encrypt` is `xor_cipher` with one extra rule: reject a key shorter than the message.

### Why nobody uses it in practice

- The key must be as long as all data you will ever send, and both sides need it in advance.
  If you can deliver that much key securely, you could have just delivered the message.
- **The key must never be reused.** That rule is broken surprisingly often in real systems.

### The key reuse attack (in the tests)

Encrypt two messages with the same key:

```
c1 = p1 ^ k
c2 = p2 ^ k

c1 ^ c2 = (p1 ^ k) ^ (p2 ^ k) = p1 ^ p2 ^ (k ^ k) = p1 ^ p2
```

**The key cancels out.** The attacker, who only sees c1 and c2, gets `p1 ^ p2`. If they can
guess any part of p1 (a header, a greeting, a known format), they get that part of p2 for free.
This really happened: the Soviet one-time pads were reused during WWII, and the US
**VENONA** project decrypted thousands of messages because of it. The same mistake
("nonce reuse") appears in modern ciphers too, which you will see in Step 6 (CTR mode).

## Hex conversion (`hex_encode`, `hex_decode`)

Every crypto standard (AES, SHA-256) publishes its test values in hex, so you need this from
Step 5 on. It is also great practice for bit operations.

One byte = 8 bits = two 4-bit halves (called **nibbles**). Each nibble is 0..15 = one hex digit.

```
0xAB = 1010 1011
       ^^^^ ^^^^
       high low
       A=10 B=11   -> "ab"
```

- Getting the high nibble: shift right by 4 (`>>`)
- Getting the low nibble: keep only the bottom 4 bits (`&` with a mask)
- Combining two nibbles back into a byte: shift left (`<<`) and OR (`|`)
- Digit to character: you need a mapping 0..15 -> '0'..'9','a'..'f'. Think about how
  `"0123456789abcdef"` could help.
- Character to digit: three ranges ('0'-'9', 'a'-'f', 'A'-'F'), anything else is invalid.

## Implementation requirements

The interface is in `xor_cipher.h`. What the tests check:

| Function | Checks |
|---|---|
| `xor_cipher` | Truth table, key 0x00/0xFF, x^x=0, known "ICE" test vector, 0x00 inside data, in-place (out == in), errors |
| `otp_encrypt` | Exact-length key, longer key, shorter key rejected, **key reuse attack demo** |
| `hex_encode` | Lowercase output, all byte values, NUL-terminated, buffer needs 2n+1 |
| `hex_decode` | Upper and lowercase, round trip of all 256 bytes, odd length / invalid chars / "0x" rejected |

## Running

```bash
make test-03
make test-03 SAN=1
```

Suggested order: `xor_cipher` -> `otp_encrypt` -> `hex_encode` -> `hex_decode`.
The test output is split into sections by function, so you can focus on one at a time.

## Pitfalls to think about

1. Why `uint8_t` and not `char`? What happens if you XOR two `char`s that are negative?
2. `out_size < in_len`: why is there no `+ 1` this time, when Steps 1 and 2 had one?
3. In-place (`out == in`): does your loop read `in[i]` before writing `out[i]`? Is it safe?
4. `otp_encrypt` is almost the same as `xor_cipher`. Can you avoid copying the code?
5. `hex_decode`: what should `"0x12"` give? Why is it treated as invalid?

## When you are done (optional)

- **Single-byte XOR cracker**: given hex ciphertext encrypted with ONE unknown key byte,
  try all 256 keys and pick the result that looks most like English (Step 1's frequency idea).
- Encrypt two English sentences with the same OTP key, XOR the ciphertexts, and try
  "crib dragging": slide a common word like `" the "` across `c1 ^ c2` and look for readable text.
