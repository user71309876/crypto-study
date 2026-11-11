/*
 * crypto_common.h - Return codes shared by every step
 *
 * Functions write their result into the out buffer and report
 * success/failure through the return value.
 * (The C standard library and real libraries like OpenSSL often work this way.)
 */
#ifndef CRYPTO_COMMON_H
#define CRYPTO_COMMON_H

#define CRYPTO_OK          0   /* Success */
#define CRYPTO_ERR_NULL   -1   /* A NULL pointer was passed as an argument */
#define CRYPTO_ERR_BUFFER -2   /* Output buffer is too small for the result */
#define CRYPTO_ERR_KEY    -3   /* The key is invalid */

#endif /* CRYPTO_COMMON_H */
