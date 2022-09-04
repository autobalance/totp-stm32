/*******

    Common headers, defines/constants, utilities/functions, etc...

*******/

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

#define ceil_div(m, n) ((m) + ((n) - 1)) / (n)

#define BITS_PER_BYTE 8U
#define BITS_PER_BASE32 5U

#define BYTES_PER_U32 4U
#define BITS_PER_U32 (BYTES_PER_U32 * BITS_PER_BYTE)
#define BYTES_PER_U64 8U
#define BITS_PER_U64 (BYTES_PER_U64 * BITS_PER_BYTE)

#define BASE32_NCHARS 33U

#define TOTP_CODE_LEN 6
#define TOTP_TIME_STEP 30 // defined as 'X' in RFC6238; interval of time for single TOTP code

char char_to_upper(char c);

void u64_to_u8be(uint8_t *out, uint64_t in);

void u32_to_u8be(uint8_t *out, uint32_t in);

uint32_t u8be_to_u32(const uint8_t *in);

uint32_t u32_rotl(uint32_t in, uint32_t shift);

#endif
