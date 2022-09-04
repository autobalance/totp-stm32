#include "base32.h"

const char base32_chars[BASE32_NCHARS] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";

// case insensitive implementation (assumes ASCII codepage)
uint8_t base32_to_binary(char c)
{
    c = char_to_upper(c);

    uint8_t bin = -1; // will modulo to 0 when loop starts
    while ((base32_chars[++bin] != c) &&
           (bin != BASE32_NCHARS));

    return bin; // returns out of bounds binary if 'c' is not base32
}

uint32_t base32_decode_len(uint32_t in_len)
{
    // units are bytes per base32 char
    return ceil_div(in_len * BITS_PER_BASE32, BITS_PER_BYTE);
}

uint32_t base32_decode(uint8_t *out, const char *in, uint32_t in_len)
{
    uint32_t out_len = 0;
    uint32_t out_shiftreg = 0;

    for (uint32_t i = 0, bits_available = 0; i < in_len; i++)
    {
        uint8_t binary = base32_to_binary(in[i]);

        if (binary >= BASE32_NCHARS-1) // break if pad char or invalid char seen
        {
            if (binary > BASE32_NCHARS-1) // output invalid since input char is
            {
                out_len = 0;
            }
            break;
        }

        out_shiftreg <<= BITS_PER_BASE32;
        out_shiftreg |= binary;

        bits_available += BITS_PER_BASE32;
        if (bits_available >= BITS_PER_BYTE)
        {
            uint8_t out_byte = (out_shiftreg >> (bits_available - BITS_PER_BYTE)) & 0xff;
            out[out_len++] = out_byte;

            bits_available -= BITS_PER_BYTE;
        }
    }

    return out_len;
}
