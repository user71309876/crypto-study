![tests](https://github.com/내아이디/crypto-study/actions/workflows/test.yml/badge.svg)

# Learning Cryptographic Algorithms in C

Each step folder contains the following:

| File        | Contents                                                 | Edit it?           |
| ----------- | -------------------------------------------------------- | ------------------ |
| `README.md` | How the algorithm works, security concepts, requirements | Read               |
| `*.h`       | Function interface (rules described in comments)         | Do not modify      |
| `*.c`       | Empty function stubs                                     | **Implement here** |
| `test_*.c`  | Tests                                                    | Do not modify      |

## How to proceed

1. Read the step's `README.md` and work through the example by hand.
2. Read the comments in the `.h` file to see the rules the functions must follow.
3. Implement the `.c` file.
4. Run the tests, e.g. `make test-01`, and fix things until there are 0 failures.
5. Run `make test-01 SAN=1` to check there are no memory errors.

## Commands

```bash
make test       # test every step
make test-01    # Step 1 only
make test-02    # Step 2 only
make clean      # delete build output
```

Works with gcc and make on Linux, macOS, and WSL.
(On macOS it's fine even if `gcc` points to clang.)

## Roadmap

| Step | Algorithm                        | Key concepts                                    | Status |
| ---- | -------------------------------- | ----------------------------------------------- | ------ |
| 1    | Caesar cipher                    | Substitution, key space, brute force            | Ready  |
| 2    | Vigenere cipher                  | Polyalphabetic substitution, frequency analysis | Ready  |
| 3    | XOR / one-time pad               | Bitwise operations, danger of key reuse         |        |
| 4    | Mini block cipher                | Rounds, confusion and diffusion                 |        |
| 5    | AES-128                          | S-box, key expansion, standard test vectors     |        |
| 6    | Modes of operation (ECB/CBC/CTR) | IV, padding                                     |        |
| 7    | SHA-256                          | Hashing, integrity                              |        |
| 8    | RSA (small numbers)              | Public key, modular exponentiation              |        |

The implementations here are for learning. Use proven libraries such as OpenSSL or libsodium in real products.
