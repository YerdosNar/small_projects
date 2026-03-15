#ifndef MAC_H
#define MAC_H

#include <stdint.h>
#include <stdio.h>

uint32_t generate_mac(const uint8_t *data, size_t len);

#endif
