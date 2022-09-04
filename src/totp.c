#include <totp.h>

void totp_get_code(char *code, const uint8_t *key, uint32_t key_len, uint64_t time)
{
    if (!key || !code)
    {
        return;
    }

    // the 30s interval of time stored as 64bit big endian
    uint8_t interval[BYTES_PER_U64];
    u64_to_u8be(interval, time / TOTP_TIME_STEP);

    uint8_t hash[SHA1_DIGEST_LEN];
    crypto_hmac_sha1(hash, key, key_len, interval, BYTES_PER_U64);

    uint32_t offset = hash[SHA1_DIGEST_LEN - 1] & 0xf;

    hash[offset] &= 0x7f;
    uint32_t totp = u8be_to_u32(&hash[offset]);

    // convert digits into ASCII
    code[TOTP_CODE_LEN] = '\0';
    for (int i = TOTP_CODE_LEN-1; i >= 0; totp /= 10, i--)
    {
        code[i] = (totp % 10) + '0';
    }
}


