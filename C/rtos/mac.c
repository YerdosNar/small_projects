#include <stdint.h>
#include <string.h>
#include <stdio.h>

const uint8_t SECRET_KEY[16] = {
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C,
    0x0D, 0x0E, 0x0F, 0x10
};

uint32_t generate_mac(const uint8_t *data, size_t len) {
    uint32_t hash = 0x811C9DC5; // Forler-Noll-Vo offset

    // First we mix the secret key
    for (int i = 0; i < 16; i++) {
        hash ^= SECRET_KEY[i];
        hash *= 0x01000193;
    }

    // Now we mix actual data
    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 0x01000193;
    }

    return hash;
}
