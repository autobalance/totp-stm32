/*******

    Simple implementation for storing TOTP name/key pairs.
    TODO: implement something like EEPROM on flash to load/store data from.

*******/

#ifndef __STORAGE_H__
#define __STORAGE_H__

#include <common.h>

typedef struct storage_struct
{
    char *name;
    uint8_t *key;
    uint32_t key_len;
} storage_data_t;

/* Retrieve numbed of data items currently stored. */
uint32_t storage_num_stored(void);

/* Retrieve data item 'n'  in storage. */
void storage_get(storage_data_t **data, uint32_t n);

/* Using compile time stored codes for now, implement these later. */
//void storage_put(uint8_t *name, uint8_t *key, uint32_t key_len;
//void storage_put_base32(uint8_t *name, char *base32_key, uint32_t base32_key_len);

#endif
