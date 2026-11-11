# Step 1: Caesar Cipher

## How it works

A **substitution cipher** that shifts each letter of the alphabet forward by a fixed number of positions (the key).
It is named after Julius Caesar, who is said to have used it for military messages.

If we turn letters into numbers (A=0, B=1, ..., Z=25), it can be written as a formula:

```
Encrypt:  C = (P + K) mod 26
Decrypt:  P = (C - K) mod 26
```

- `P`: plaintext letter number, `C`: ciphertext letter number, `K`: key
- `mod 26`: remainder after dividing by 26. It wraps around so that A comes after Z.

### Example (K = 3)

```
Plain:  A B C ... W X Y Z
        | | |     | | | |
Cipher: D E F ... Z A B C
```

`"ATTACK"` -> `"DWWDFN"`

### Try it by hand first

What is `"XYZ"` encrypted with key 3?
- X = 23 -> (23 + 3) mod 26 = 0 -> A
- Y = 24 -> (24 + 3) mod 26 = 1 -> B
- Z = 25 -> (25 + 3) mod 26 = 2 -> C

## Security concepts in this step

- **Symmetric-key cipher**: encryption and decryption use the same key. Decryption is the inverse of encryption.
- **Key space**: the number of possible keys. The Caesar cipher has only 25 meaningful keys.
- **Brute-force attack**: if the key space is small, trying every key breaks it immediately.
  AES-128 has a key space of 2^128 (about 3.4 x 10^38), so this attack is impossible.
- **ROT13**: a Caesar cipher with key 13. Since 13 is half of 26, applying it twice gives back the original.

## Implementation requirements

The interface is described in the comments in `caesar.h`. What the tests check:

| Item | Description |
|---|---|
| Basic | `"ABC"` + 3 -> `"DEF"` |
| Wrap-around | `"XYZ"` + 3 -> `"ABC"` |
| Case preserved | Uppercase stays uppercase, lowercase stays lowercase |
| Non-letters | Spaces, digits, symbols unchanged |
| Key range | 0, 26, 29, -1, -27 all work correctly |
| Round trip | For every key from -30 to 30, encrypt -> decrypt = original |
| Errors | NULL pointers and too-small buffers reported via return value |

## Running

```bash
make test-01          # run tests
make test-01 SAN=1    # run with the memory error checker enabled
```

## Pitfalls to think about

Common traps when implementing this in C. Check them yourself.

1. What is the result of `-1 % 26` in C? Print it with `printf`.
2. What happens if you store `'z' + 3` in a `char`? What is the range of `char`?
3. When checking the buffer size, did you count the `'\0'` at the end of the string?

## When you are done (optional)

- Write a `crack_caesar` program that takes ciphertext with an unknown key, tries all 25 keys, and prints each result.
- Go further: make the program pick the result that **looks like English** by itself.
  (Hint: the most common letters in English are E, T, A in that order.) This is the
  **frequency analysis** used in Step 2.
