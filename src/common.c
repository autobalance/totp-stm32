#include <common.h>

char char_to_upper(char c)
{
    if ('a' <= c && c <= 'z')
    {
        c = (c - 'a') + 'A';
    }

    return c;
}

void u64_to_u8be(uint8_t *out, uint64_t in)
{
    int i_begin = BYTES_PER_U64 - 1;
    for (int i = i_begin; i >= 0; i--)
    {
        out[i] = in & 0xff;
        in >>= 8;
    }
}

void u32_to_u8be(uint8_t *out, uint32_t in)
{
    int i_begin = BYTES_PER_U32 - 1;
    for (int i = i_begin; i >= 0; i--)
    {
        out[i] = in & 0xff;
        in >>= 8;
    }
}

uint32_t u8be_to_u32(const uint8_t *in)
{
    uint32_t out = 0;

    int i_begin = BYTES_PER_U32 - 1;
    for (int i = i_begin; i >= 0; i--)
    {
        out |= ((uint32_t) in[i]) << ((i_begin - i) * BITS_PER_BYTE);
    }

    return out;
}

uint32_t u32_rotl(uint32_t in, uint32_t shift)
{
    if (shift >= 32)
    {
        return in;
    }

    return (in << shift) | (in >> (BITS_PER_U32 - shift));
}
