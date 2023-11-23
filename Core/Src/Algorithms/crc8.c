#include "Algorithms/crc8.h"

uint32_t crc8(uint32_t crc, unsigned char const *data, size_t len)
{
    if (data == NULL) {
    	return 0;
    }
    crc &= 0xff;
    unsigned char const *end = data + len;
    while (data < end) {
        crc = crc8_table[crc ^ *data++];
    }
    return crc;
}
