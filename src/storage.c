#include <storage.h>
#include <base32.h>

#define NUM_STORED 3

// designed to work with a LCD/microcontroller, change as desired
#define STORAGE_NAME_MAXLEN 16  // allocate enough for name to fit in one row of LCD
#define STORAGE_KEY_MAXLEN 64   // allocate for at most 64-char base32 keys (store in base32)

// stored as base32 in here, but converted to binary before retrieval
// TODO: implement user interaction to enter codes outside (e.g. using rotary and buttons)
const storage_data_t storage_data[NUM_STORED] =
{
    { .name = "An account", .key = (uint8_t *) "ABCDEFGHI=", .key_len = 10 },
    { .name = "Another account", .key = (uint8_t *) "234WHATAKEY67", .key_len = 13 },
    { .name = "One more?", .key = (uint8_t *) "WHYNOTwha7saKEYFO4", .key_len = 18 }
};

// a single place to get retrieved data (key_ret in binary)
// should be volatile?
char name_ret[STORAGE_NAME_MAXLEN];
uint8_t key_ret[(STORAGE_KEY_MAXLEN * BITS_PER_BASE32) / BITS_PER_BYTE];

storage_data_t ret = { .name = name_ret, .key = key_ret, .key_len = 0 };

uint32_t storage_num_stored(void)
{
    return NUM_STORED;
}

void storage_get(storage_data_t **data, uint32_t n)
{
    if (!data)
    {
        return;
    }
    else if (n >= NUM_STORED)
    {
        *data = 0;
    }

    for (int i = 0; i < STORAGE_NAME_MAXLEN; i++)
    {
        name_ret[i] = storage_data[n].name[i];
    }

    ret.key_len = base32_decode(key_ret, (char *) (storage_data[n].key), storage_data[n].key_len);

    *data = &ret;
}
