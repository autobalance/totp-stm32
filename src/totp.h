/*******

    A bare-bones TOTP implementation using only SHA1 for now.
    Derived from RFC6238: https://www.rfc-editor.org/rfc/rfc6238

*******/

#ifndef __TOTP_H__
#define __TOTP_H__

#include <common.h>
#include <crypto.h>

// designed to work with a LCD/microcontroller, change as desired
#define TOTP_NAME_MAXLEN 16  // allocate enough for name to fit in one row of LCD
#define TOTP_KEY_MAXLEN 64   // allocate for at most 64-char base32 keys (store in base32)

/* Requires 'code' to have 7 bytes pre-allocated. */
/* 'key' is base-2 binary. 'time' should be seconds since UNIX epoch. */
void totp_get_code(char *code, const uint8_t *key, uint32_t key_len, uint64_t time);

#endif
