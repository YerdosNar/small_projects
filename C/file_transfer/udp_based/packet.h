#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

// Safe MTU size (Ethernet is usually 1500, we stay under to be safe)
#define MAX_PACKET_SIZE 1400

#define TYPE_METADATA 1  // Contains filename/filesize
#define TYPE_DATA     2  // Contains file content
#define TYPE_EOF      3  // End of transmission

// The "Head" of the packet.
// __attribute__((packed)) prevents the compiler from adding gaps between fields.
typedef struct __attribute__((packed)) {
    uint32_t type;        // Message type
    uint32_t seq_num;     // Sequence number
    uint32_t data_len;    // Length of USEFUL data (receiver ignores bytes after this)
    uint32_t padding_len; // Length of GARBAGE data (just for traffic shaping)
} PacketHeader;

#endif
