#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

// Message types
#define MSG_TEXT         0x01  // Text chat message
#define MSG_FILE_REQ     0x02  // File transfer request (filename + size)
#define MSG_FILE_ACCEPT  0x03  // Accept file transfer
#define MSG_FILE_REJECT  0x04  // Reject file transfer
#define MSG_FILE_DATA    0x05  // File chunk
#define MSG_FILE_DONE    0x06  // File transfer complete
#define MSG_FILE_PORT    0x07  // Port for file transfer connection

#define MAX_MSG_LEN      4096
#define MAX_FILENAME     256
#define CHUNK_SIZE       8192

// Message header (sent before every message)
typedef struct {
    uint8_t  type;           // Message type
    uint32_t length;         // Payload length
} __attribute__((packed)) msg_header_t;

// File request payload
typedef struct {
    char     filename[MAX_FILENAME];
    uint64_t filesize;
    uint32_t transfer_id;    // Unique ID for this transfer
} __attribute__((packed)) file_request_t;

// File data chunk header
typedef struct {
    uint32_t transfer_id;
    uint32_t seq_num;
    uint32_t chunk_size;
} __attribute__((packed)) file_chunk_header_t;

// File transfer state
typedef struct {
    uint32_t transfer_id;
    char     filename[MAX_FILENAME];
    uint64_t filesize;
    uint64_t transferred;
    FILE     *fp;
    int      active;
    int      is_sender;
    int      data_socket;    // Separate socket for file data
    uint16_t data_port;      // Port for file data connection
} file_transfer_t;

#define MAX_TRANSFERS 8

#endif
