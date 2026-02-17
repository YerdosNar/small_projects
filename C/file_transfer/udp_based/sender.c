#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "utils.h"

void usage(char *exe) {
    printf("Usage: %s [options]\n", exe);
    printf("\n");
    printf("Options:\n");
    printf("    -i, --ip            Receiver's IP\n");
    printf("    -s, --s-port        Sender's PORT\n");
    printf("    -r, --r-port        Receiver's PORT\n");
    printf("    -f, --filename      File to send\n");
    printf("    -h, --help          This message\n");
    exit(0);
}

int main(int argc, char **argv) {
    // Initial variables
    char ip_buf[INET_ADDRSTRLEN];
    char filename_buf[128];

    uint32_t sender_port;
    char     *receiver_ip;
    uint32_t receiver_port;
    char     *filename;

    if(argc < 2) {
        printf("Enter your port: ");
        scanf("%u", &sender_port);
        getchar(); // leftover newline from scanf

        printf("Enter receiver's PORT: ");
        scanf("%u", &receiver_port);
        getchar(); // the same, consume leftover '\n'

        printf("Enter receiver's IP: ");
        if(fgets(ip_buf, sizeof(ip_buf), stdin)) {
            ip_buf[strcspn(ip_buf, "\n")] = 0;
            receiver_ip = ip_buf;
        }

        printf("Enter filename to send: ");
        if(fgets(filename_buf, sizeof(filename_buf), stdin)) {
            filename_buf[strcspn(filename_buf, "\n")] = 0;
            filename = filename_buf;
        }
    }
    else if((argc > 2 && argc < 9) || !strncmp("-h", argv[1], 2) || !strncmp("--help", argv[1], 6)) {
        usage(argv[0]);
    }
    else {
        for(int i = 1; i < argc; i++) {
            if((!strncmp("-f", argv[i], 2) || !strncmp("--filename", argv[i], 10)) && i+1 < argc) {
                filename = argv[i+1];
            }
            else if((!strncmp("-i", argv[i], 2) || !strncmp("--ip", argv[i], 4)) && i+1 < argc) {
                receiver_ip = argv[i+1];
            }
            else if((!strncmp("-s", argv[i], 2) || !strncmp("--s-port", argv[i], 8)) && i+1 < argc) {
                sender_port = atoi(argv[i+1]);
            }
            else if((!strncmp("-r", argv[i], 2) || !strncmp("--r-port", argv[i], 8)) && i+1 < argc) {
                receiver_port = atoi(argv[i+1]);
            }
        }
    }
    info("Socket creating...");
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) err("Socket creation failed");
    sccs("Socket created!");


    struct sockaddr_in sender_addr  = {0};
    sender_addr.sin_family          = AF_INET;
    sender_addr.sin_port            = htons(sender_port);
    sender_addr.sin_addr.s_addr     = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr *)&sender_addr, sizeof(sender_addr)) < 0) {
        err("bind() failed");
    }

    struct sockaddr_in receiver_addr = {0};
    receiver_addr.sin_family         = AF_INET;
    receiver_addr.sin_port           = htons(receiver_port);
    if(0 >= inet_pton(AF_INET, receiver_ip, &receiver_addr.sin_addr.s_addr)) {
        warn("Invalid IP");
        err("inet_pton(receiver_ip) failed");
    }

    socklen_t addrlen = sizeof(receiver_addr);

    FILE *fp = fopen(filename, "r");
    if(!fp) err("File '%s' failed to open", filename);

    // Get file size
    if(fseek(fp, 0L, SEEK_END) == -1) err("fseek() failed");
    uint64_t file_size = ftell(fp);
    rewind(fp);

    uint8_t metadata_send[256];
    snprintf((char*)metadata_send, sizeof(metadata_send)-1, "%s|%lu", filename, file_size);
    packet_t metadata_pkt = {.type = T_DAT, .seq_num = 0};
    memcpy(metadata_pkt.data, metadata_send, sizeof(metadata_send));

    // Send metadata first
    info("Sending metadata: %s", metadata_pkt.data);
    if(sendto(sockfd, &metadata_pkt, sizeof(metadata_pkt), 0, (struct sockaddr*)&receiver_addr, addrlen) < 0) {
        err("sendto(receiver_addr) failed");
    }
    sccs("Sent: '%s' to %s:%d", metadata_pkt.data,
         inet_ntoa(receiver_addr.sin_addr), receiver_port);

    // Receive acknowledge
    packet_t ack_pkt = {0};
    if(recvfrom(sockfd, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr*)&receiver_addr, &addrlen) < 0) {
        err("recvfrom(receiver_addr) failed");
    }

    if(memcmp(ack_pkt.data, metadata_send, sizeof(metadata_send))) {
        warn("Received: %s", ack_pkt.data);
        warn("Expected: %s", metadata_send);
    }
    else {
        sccs("Received ACK successfully");
    }

    info("Sending body of the file");
    uint32_t    seq_num     = 1;
    int8_t      last_percent= -1;
    uint64_t    sum_read    = 0;

    while(1) {
        packet_t send_pkt = {
            .type         = T_DAT,
            .seq_num      = htonl(seq_num++)
        };

        // Read 1 byte DATA_SIZE times
        size_t bytes_read = fread(send_pkt.data, 1, DATA_SIZE, fp);
        if(bytes_read     < DATA_SIZE) {
            send_pkt.type = T_EOF;
            if(ferror(fp)) err("File read error");
        }

        uint32_t payload_size = sizeof(send_pkt.type) + sizeof(send_pkt.seq_num) + bytes_read;
        ssize_t sent_now = sendto(sockfd, &send_pkt, payload_size, 0, (struct sockaddr*)&receiver_addr, addrlen);
        if(sent_now < 0) {
            err("sendto(seq_num=%d) failed", seq_num);
        }

        sum_read += bytes_read;
        int8_t curr_percent = sum_read * 100 / file_size;
        if(last_percent < curr_percent) {
            last_percent = curr_percent;
            print_progress_bar(last_percent);
        }

        if(send_pkt.type == T_EOF) {
            printf("\n");
            sccs("File sent successfully");
            break;
        }
    }

    fclose(fp);
    close(sockfd);

    return 0;
}
