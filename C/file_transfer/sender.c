#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "packet.h"

void send_packet(int sockfd, struct sockaddr_in *addr, socklen_t addrlen,
                 int type, int seq, void *data, int data_len, int target_total_size);

int main(int argc, char **argv) {
    if(argc < 5) {
        fprintf(stderr, "Usage: %s <sender_port> <receiver_ip> <receiver_port> <filename>\n", argv[0]);
        return 1;
    }

    // Seed random number generator
    srand(time(NULL));

    int sender_port = atoi(argv[1]);
    char *receiver_ip = argv[2];
    int receiver_port = atoi(argv[3]);
    char *filename = argv[4];

    // --- 1. Network Setup ---
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) { perror("Socket creation failed"); return 1; }

    struct sockaddr_in sender_addr = {0};
    sender_addr.sin_family = AF_INET;
    sender_addr.sin_addr.s_addr = INADDR_ANY;
    sender_addr.sin_port = htons(sender_port);

    if(bind(sockfd, (struct sockaddr *)&sender_addr, sizeof(sender_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    struct sockaddr_in receiver_addr = {0};
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(receiver_port);
    if(inet_pton(AF_INET, receiver_ip, &receiver_addr.sin_addr) <= 0) {
        perror("Invalid IP");
        return 1;
    }
    socklen_t addrlen = sizeof(receiver_addr);

    // --- 2. File Setup ---
    FILE *fp = fopen(filename, "rb");
    if(!fp) { perror("File open failed"); return 1; }

    // Get file size for metadata
    struct stat st;
    stat(filename, &st);
    int file_size = st.st_size;

    // --- 3. Send Metadata (First Packet) ---
    // We send filename + filesize so receiver knows what to expect
    char metadata_buf[256];
    snprintf(metadata_buf, sizeof(metadata_buf), "%s|%d", filename, file_size);

    // Send metadata packet (Fixed size or random, doesn't matter much for first one)
    send_packet(sockfd, &receiver_addr, addrlen, TYPE_METADATA, 0,
                metadata_buf, strlen(metadata_buf), 1024);

    printf("[+] Sending %s (%d bytes)...\n", filename, file_size);

    // --- 4. Main Data Loop (The Obfuscation Logic) ---
    int seq = 1;
    int packets_until_resize = 0;
    int current_target_size = 0;
    uint8_t file_buffer[MAX_PACKET_SIZE]; // Temporary hold for file data

    while(1) {
        // A. LOGIC: Change packet size profile every N packets
        // This simulates natural variance in network traffic
        if (packets_until_resize <= 0) {
            // Pick a new target size between 800 and MAX_PACKET_SIZE bytes
            current_target_size = (rand() % (MAX_PACKET_SIZE - 800)) + 800;
            // Keep this "profile" for 5 to 20 packets
            packets_until_resize = (rand() % 15) + 5;
        }

        // B. Calculate Space
        int header_size = sizeof(PacketHeader);
        // Reserve at least 50 bytes for padding to ensure obfuscation
        int max_data_capacity = current_target_size - header_size - 50;

        // C. Read Data
        int bytes_read = fread(file_buffer, 1, max_data_capacity, fp);

        if (bytes_read <= 0) break; // EOF

        // D. Send Obfuscated Packet
        // pass 'current_target_size' to force the function to pad the rest
        send_packet(sockfd, &receiver_addr, addrlen, TYPE_DATA, seq++,
                    file_buffer, bytes_read, current_target_size);

        packets_until_resize--;

        // Optional: Small sleep to prevent UDP packet loss on localhost
        usleep(500);
    }

    // --- 5. Finish ---
    send_packet(sockfd, &receiver_addr, addrlen, TYPE_EOF, seq, NULL, 0, 512);
    printf("\n[+] Transfer Complete.\n");

    fclose(fp);
    close(sockfd);
    return 0;
}

// --- Helper Function to Build and Send ---
void send_packet(int sockfd, struct sockaddr_in *addr, socklen_t addrlen,
                 int type, int seq, void *data, int data_len, int target_total_size) {

    // 1. Create the buffer (The "Envelope")
    // We use a stack buffer. No malloc = No leaks.
    uint8_t packet_buffer[MAX_PACKET_SIZE];

    // 2. Clear buffer (optional, but good for debugging)
    memset(packet_buffer, 0, target_total_size);

    // 3. Prepare Header
    PacketHeader *hdr = (PacketHeader*)packet_buffer;

    // Calculate padding needed to reach target size
    int header_size = sizeof(PacketHeader);
    int current_filled = header_size + data_len;
    int padding_needed = 0;

    if (target_total_size > current_filled) {
        padding_needed = target_total_size - current_filled;
    }

    // Fill Header (Use htonl for Network Byte Order!)
    hdr->type = htonl(type);
    hdr->seq_num = htonl(seq);
    hdr->data_len = htonl(data_len);
    hdr->padding_len = htonl(padding_needed);

    // 4. Copy Data (if any)
    if (data_len > 0 && data != NULL) {
        // Copy data right after the header
        memcpy(packet_buffer + header_size, data, data_len);
    }

    // 5. Fill Padding with Random Garbage
    // Start writing padding after data
    uint8_t *padding_ptr = packet_buffer + header_size + data_len;
    for (int i = 0; i < padding_needed; i++) {
        padding_ptr[i] = rand() % 256; // Random byte
    }

    // 6. Send the total size
    int total_to_send = header_size + data_len + padding_needed;

    sendto(sockfd, packet_buffer, total_to_send, 0,
           (struct sockaddr *)addr, addrlen);
}
