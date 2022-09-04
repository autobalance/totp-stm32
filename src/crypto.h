/*******

    A very minimalist implementation of crypto functions for TOTP.
    Implemented by reference from Paar and Pelzl's 'Understanding Cryptography':
    https://link.springer.com/book/10.1007/978-3-642-04101-3

*******/

#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include "common.h"

#define SHA1_BLOCK_BITLEN 512U
#define SHA1_DIGEST_BITLEN 160U

// byte length of constans above
#define SHA1_BLOCK_LEN ((SHA1_BLOCK_BITLEN) / BITS_PER_BYTE)
#define SHA1_DIGEST_LEN ((SHA1_DIGEST_BITLEN) / BITS_PER_BYTE)

/* A simple implementation of SHA1 (see Section 11.4 in 'Understanding Cryptography') */
/* TODO: make it progressive (e.g. separate into 'init, update, finalize' functions */
void crypto_sha1(uint8_t *hash, const uint8_t *msg, uint64_t msg_len);

/* A simple implementation of HMAC-SHA1 (see Section 12.2 in 'Understanding Cryptography') */
/* TODO: make it progressive (e.g. separate into 'init, update, finalize' functions */
void crypto_hmac_sha1(uint8_t *hmac, const uint8_t *key, uint32_t key_len, const uint8_t *msg, uint64_t msg_len);

#endif
