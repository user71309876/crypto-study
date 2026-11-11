# Step 2: Vigenere Cipher

## How it fixes the weaknesses of Step 1

The Caesar cipher is easy to break for two reasons:

1. There are only 25 keys, so you can try them all.
2. The same plaintext letter always becomes the same ciphertext letter. So you can guess
   that the most common letter in the ciphertext is probably E (**frequency analysis**).

The Vigenere cipher uses a **word (keyword)** as the key instead of a single number, and shifts
each letter by a different amount. Using several substitution alphabets in turn like this is called
**polyalphabetic substitution**. It was devised in the 16th century and was called
"the indecipherable cipher" for about 300 years.

## How it works

Each letter of the keyword is turned into a number and used as a shift (A=0, B=1, ..., Z=25).
If the keyword is shorter than the plaintext, it repeats from the start.

```
Encrypt:  C[i] = (P[i] + K[i mod keylen]) mod 26
Decrypt:  P[i] = (C[i] - K[i mod keylen]) mod 26
```

### Example: plaintext `ATTACKATDAWN`, key `LEMON`

```
Plain       A  T  T  A  C  K  A  T  D  A  W  N
Key repeat  L  E  M  O  N  L  E  M  O  N  L  E
Shift      11  4 12 14 13 11  4 12 14 13 11  4
Cipher      L  X  F  O  P  V  E  F  R  N  H  R
```

The four `A`s in the plaintext became `L`, `O`, `E`, and `N`. Since the same letter turns into
different letters, simple frequency analysis no longer works.

### Relation to the Caesar cipher

With a key of length 1 it is exactly the Caesar cipher. Key `"D"` = Caesar key 3.
In other words, the Vigenere cipher is **several Caesar ciphers used in turn**.
Think about how you could reuse the logic you wrote in Step 1.

## Rule for this step: handling non-letters

Spaces and symbols are left unchanged, and **the key position does not advance.**

```
Plain    A  t  t  a  c  k     a  t     D  a  w  n  !
Key      L  E  M  O  N  L     E  M     O  N  L  E
Cipher   L  x  f  o  p  v     e  f     R  n  h  r  !
```

If a space consumed the key, the result would be different, so be careful. The tests check this.

## Security concepts in this step

- **Larger key space**: there are 26^n keywords of length n. Length 10 gives about 1.4 x 10^14.
- **Why it still breaks**: because the key **repeats**. Once you know the key length, split the
  ciphertext into groups by key length, treat each group as a Caesar cipher, and apply frequency analysis.
  (Techniques: the Kasiski examination and the Index of Coincidence)
- **Lesson**: a large key space does not mean it is secure. If **patterns** remain in the ciphertext,
  it can be attacked. This is why modern ciphers care about "confusion" and "diffusion" (Step 4).
- **What if the key is as long as the plaintext, never repeats, and is completely random?**
  That is the one-time pad in Step 3.

## Implementation requirements

The interface is described in the comments in `vigenere.h`. What the tests check:

| Item | Description |
|---|---|
| Textbook example | `ATTACKATDAWN` + `LEMON` -> `LXFOPVEFRNHR` |
| Key of length 1 | Same result as the Caesar cipher |
| Key case | `"lemon"` and `"LeMoN"` are the same as `"LEMON"` |
| Plaintext case | Preserved |
| Non-letters | Left unchanged and do not consume the key |
| Long key | Works with a key longer than the plaintext |
| Errors | NULL, empty key, key with non-letters, buffer too small |
| Error order | Check NULL -> key -> buffer, in that order |

## Running

```bash
make test-02
make test-02 SAN=1
```

## Pitfalls to think about

1. Should the key index and the plaintext index be the same variable or separate ones?
2. When should you validate the key? If you validate while converting, a half-written result is
   left in `out` on error. Is that OK?
3. Can you build decryption by reusing the encrypt function? Unlike Caesar, the key is a string,
   so `-key` does not work.

## When you are done (optional)

- **Key length estimator**: write a program that takes ciphertext and estimates the key length.
  Compute the Index of Coincidence (IC) and pick the length whose IC is closest to that of
  English plaintext (about 0.066).
- Once you know the key length, apply Step 1's frequency analysis to each position to recover the whole key.
