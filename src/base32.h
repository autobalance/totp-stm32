/*******

    A Base32 (non-hex) decoder (TODO: encoder) implementation.
    Derived from RFC4648: https://www.rfc-editor.org/rfc/rfc4648

*******/

#ifndef __BASE32_H__
#define __BASE32_H__

#include <common.h>

/* Maximum length of decoded output given length of Base32 input */
uint32_t base32_decode_len(uint32_t in_len);

/* 'out' should have pre-allocated space in the size given by 'base32_decode_len' above */
uint32_t base32_decode(uint8_t *out, const char *in, uint32_t in_len);

/* TODO: implement Base32 encoding */
//uint32_t base32_encode_len(uint32_t in_len);
//uint32_t base32_encode(char *out, uint8_t *in, uint32_t in_len);

#endif
