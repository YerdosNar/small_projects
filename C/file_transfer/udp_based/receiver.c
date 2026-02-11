#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "packet.h"

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <port_to_listen_on>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    // --- 1. Network Setup ---
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) { perror("Socket creation failed"); return 1; }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    server_addr.sin_port = htons(port);

    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    printf("[*] Listening on port %d...\n", port);

    // --- 2. The Loop ---
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Using a static buffer prevents memory leaks.
    uint8_t buffer[MAX_PACKET_SIZE];

    FILE *fp = NULL;
    int total_bytes_received = 0;

    while(1) {
        // A. Receive the raw packet (Header + Data + Padding)
        int received_len = recvfrom(sockfd, buffer, MAX_PACKET_SIZE, 0,
                                   (struct sockaddr *)&client_addr, &addr_len);

        if(received_len < sizeof(PacketHeader)) {
            // Packet too small to even contain a header? Ignore it.
            continue;
        }

        // B. Parse Header (The "Unpacking")
        PacketHeader *hdr = (PacketHeader*)buffer;

        // IMPORTANT: Convert from Network Byte Order to Host Byte Order
        uint32_t type = ntohl(hdr->type);
        uint32_t seq  = ntohl(hdr->seq_num);
        uint32_t data_len = ntohl(hdr->data_len);
        uint32_t padding_len = ntohl(hdr->padding_len);

        // Calculate where the data actually starts
        uint8_t *data_ptr = buffer + sizeof(PacketHeader);

        // C. Handle Packet Types
        if (type == TYPE_METADATA) {
            // Parse "filename|filesize"
            char metadata[256];
            // Safety: ensure we don't read past the data_len
            int safe_len = (data_len < 255) ? data_len : 255;
            memcpy(metadata, data_ptr, safe_len);
            metadata[safe_len] = '\0'; // Null terminate

            // Split string at '|'
            char *filename = strtok(metadata, "|");
            char *filesize_str = strtok(NULL, "|");

            printf("[*] Incoming File: %s (Size: %s)\n", filename, filesize_str);

            // Open file for writing
            // Note: We prepend "recv_" so we don't overwrite the original if testing in same folder
            char output_filename[300];
            snprintf(output_filename, sizeof(output_filename), "recv_%s", filename);
            fp = fopen(output_filename, "wb");
            if (!fp) { perror("Failed to open file"); return 1; }

        } else if (type == TYPE_DATA) {
            if (fp) {
                // WRITE ONLY THE DATA LENGTH
                // We ignore 'received_len' and 'padding_len' here.
                fwrite(data_ptr, 1, data_len, fp);
                total_bytes_received += data_len;

                // Visualization of what happened
                printf("\r[+] Seq: %4d | Wire Size: %4d | Data: %4d | Garbage: %4d",
                       seq, received_len, data_len, padding_len);
                fflush(stdout);
            }
        } else if (type == TYPE_EOF) {
            printf("\n[!] EOF Received. Transfer complete.\n");
            break;
        }
    }

    if (fp) fclose(fp);
    close(sockfd);
    return 0;
}
