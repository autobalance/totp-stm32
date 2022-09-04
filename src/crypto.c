#include "crypto.h"

#define SHA1_MSG_SCHED_ROUND 80U
#define SHA1_STAGES 4U
#define SHA1_ROUNDS 20U

#define SHA1_A_INIT 0x67452301
#define SHA1_B_INIT 0xEFCDAB89
#define SHA1_C_INIT 0x98BADCFE
#define SHA1_D_INIT 0x10325476
#define SHA1_E_INIT 0xC3D2E1F0

#define SHA1_F_K_0 0x5A827999
#define SHA1_F_K_1 0x6ED9EBA1
#define SHA1_F_K_2 0x8F1BBCDC
#define SHA1_F_K_3 0xCA62C1D6

uint64_t padding_len(uint64_t msg_len)
{
    // computed as per section 11.4.1 in 'Understanding Crpytography'
    // unsigned arithmetic should provide positive remainder here
    uint64_t msg_bitlen = msg_len * BITS_PER_BYTE;
    uint64_t k_bits = (448UL - (msg_bitlen + 1UL)) % ((uint64_t) SHA1_BLOCK_BITLEN);

    return ceil_div(k_bits, BITS_PER_BYTE);
}

void pad_msg(uint8_t *padded_msg, uint64_t padded_len, const uint8_t *msg, uint64_t msg_len)
{
    for (uint64_t i = 0; i < msg_len; i++)
    {
        padded_msg[i] = msg[i];
    }

    padded_msg[msg_len] = 0x80; // append a 1 followed by required number of 0s
    for (uint64_t i = msg_len + 1; i < padded_len - BYTES_PER_U64; i++)
    {
        padded_msg[i] = 0;
    }

    // store length of message (in bits) in last 64 bits of padded message
    u64_to_u8be(padded_msg + (padded_len - BYTES_PER_U64), msg_len * BITS_PER_BYTE);
}

uint32_t f(uint32_t stage, const uint32_t *hash, uint32_t Wj)
{
    uint32_t ret = hash[4] + Wj + u32_rotl(hash[0], 5);

    if (stage == 0)
    {
        ret += (hash[1] & hash[2]) | (~hash[1] & hash[3]);
        ret += SHA1_F_K_0;
    }
    else if (stage == 1)
    {
        ret += hash[1] ^ hash[2] ^ hash[3];
        ret += SHA1_F_K_1;
    }
    else if (stage == 2)
    {
        ret += (hash[1] & hash[2]) | (hash[1] & hash[3]) | (hash[2] & hash[3]);
        ret += SHA1_F_K_2;
    }
    else if (stage == 3)
    {
        ret += hash[1] ^ hash[2] ^ hash[3];
        ret += SHA1_F_K_3;
    }

    return ret;
}

void sha1_pad(uint32_t *msg_hash, const uint8_t *msg, uint64_t msg_len)
{
    msg_hash[0] = SHA1_A_INIT;
    msg_hash[1] = SHA1_B_INIT;
    msg_hash[2] = SHA1_C_INIT;
    msg_hash[3] = SHA1_D_INIT;
    msg_hash[4] = SHA1_E_INIT;

    uint32_t W[80];
    for (uint64_t i = 0; i < msg_len / SHA1_BLOCK_LEN; i++)
    {
        uint32_t tmp_hash[SHA1_DIGEST_LEN / BYTES_PER_U32];
        for (uint32_t j = 0; j < SHA1_DIGEST_LEN / BYTES_PER_U32; j++)
        {
            tmp_hash[j] = msg_hash[j];
        }

        const uint8_t *x_i = msg + i*SHA1_BLOCK_LEN; // the i-th 512-bit block in message
        for (uint32_t j = 0; j < SHA1_MSG_SCHED_ROUND; j++)
        {
            if (j < 16)
            {
                W[j] = u8be_to_u32(&x_i[BYTES_PER_U32 * j]);
            }
            else
            {
                W[j] = u32_rotl(W[j-16] ^ W[j-14] ^ W[j-8] ^ W[j-3], 1);
            }
        }

        for (uint32_t stage = 0; stage < SHA1_STAGES; stage++)
        {
            for (uint32_t round = 0; round < SHA1_ROUNDS; round++)
            {
                uint32_t f_s = f(stage, tmp_hash, W[SHA1_ROUNDS*stage + round]);
                tmp_hash[4] = tmp_hash[3];               // E <- D
                tmp_hash[3] = tmp_hash[2];               // D <- C
                tmp_hash[2] = u32_rotl(tmp_hash[1], 30); // C <- B <<< 30
                tmp_hash[1] = tmp_hash[0];               // B <- A
                tmp_hash[0] = f_s;                       // A <- E + f(B,C,D) + A <<< 5 + W[j] + K_stage
            }
        }

        for (uint32_t j = 0; j < SHA1_DIGEST_LEN / BYTES_PER_U32; j++)
        {
            msg_hash[j] += tmp_hash[j];
        }
    }
}

/* only uses stack memory; useful for small hashes like for TOTP */
void crypto_sha1(uint8_t *hash, const uint8_t *msg, uint64_t msg_len)
{
    // last 'BYTES_PER_U64' is to append length of message
    uint64_t padded_msg_len = msg_len + padding_len(msg_len) + BYTES_PER_U64;
    uint8_t padded_msg[padded_msg_len];
    pad_msg(padded_msg, padded_msg_len, msg, msg_len);

    uint32_t hash_u32[SHA1_DIGEST_LEN / BYTES_PER_U32];
    sha1_pad(hash_u32, padded_msg, padded_msg_len);

    for (uint32_t i = 0; i < SHA1_DIGEST_LEN / BYTES_PER_U32; i++)
    {
        u32_to_u8be(&hash[i*BYTES_PER_U32], hash_u32[i]);
    }
}

/* also only uses stack memory */
void crypto_hmac_sha1(uint8_t *hmac, const uint8_t *key, uint32_t key_len, const uint8_t *msg, uint64_t msg_len)
{
    const uint8_t ipad = 0x36, opad = 0x5c;

    // reference https://en.wikipedia.org/wiki/HMAC for truncated key definition
    uint8_t trunc_key[SHA1_DIGEST_LEN];
    if (key_len > SHA1_BLOCK_LEN)
    {
        crypto_sha1(trunc_key, key, key_len);
        key = trunc_key;
        key_len = SHA1_DIGEST_LEN;
    }

    // build inner message
    uint8_t inner[SHA1_BLOCK_LEN + msg_len];
    for (uint32_t i = 0; i < key_len; i++)
    {
        inner[i] = key[i] ^ ipad;
    }
    for (uint32_t i = key_len; i < SHA1_BLOCK_LEN; i++)
    {
        inner[i] = ipad;
    }
    for (uint32_t i = SHA1_BLOCK_LEN; i < SHA1_BLOCK_LEN + msg_len; i++)
    {
        inner[i] = msg[i - SHA1_BLOCK_LEN];
    }

    // build outer message to concatenate with hashed inner message
    uint8_t outer[SHA1_BLOCK_LEN + SHA1_DIGEST_LEN];
    for (uint32_t i = 0; i < key_len; i++)
    {
        outer[i] = key[i] ^ opad;
    }
    for (uint32_t i = key_len; i < SHA1_BLOCK_LEN; i++)
    {
        outer[i] = opad;
    }

    // outer[64:80] = SHA1((key^ipad) || msg)
    crypto_sha1(outer + SHA1_BLOCK_LEN, inner, SHA1_BLOCK_LEN + msg_len);

    crypto_sha1(hmac, outer, SHA1_BLOCK_LEN + SHA1_DIGEST_LEN);
}

