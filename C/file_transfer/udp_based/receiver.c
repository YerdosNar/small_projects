#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "utils.h"

int main(int argc, char **argv) {
    int32_t port;
    if(argc != 2) {
        printf("Enter your port: ");
        scanf("%d", &port);
        getchar();
    }
    else {
        port = atoi(argv[1]);
    }

    info("Socket creation...");
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) err("Socket creation failed");
    sccs("Socket created");

    struct sockaddr_in receiver_addr = {0};
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(port);
    receiver_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sockfd, (struct sockaddr *)&receiver_addr, sizeof(receiver_addr)) < 0) {
        err("bind() failed");
    }

    sccs("Listening on port: %d", port);

    struct sockaddr_in sender_addr = {0};
    socklen_t addrlen = sizeof(sender_addr);

    // Metadata packet
    packet_t metadata = {0};
    ssize_t received = recvfrom(sockfd, &metadata, sizeof(metadata), 0, (struct sockaddr *)&sender_addr, &addrlen);
    if(received < 0) err("recvfrom(metadata) failed");
    sccs("Received: %s", metadata.data);

    char *filename = strtok((char*)metadata.data, "|");
    char *size_str = strtok(NULL, "|");

    if(!filename || !size_str) err("Invalied metadata");

    uint64_t file_size = strtoull(size_str, NULL, 10);
    info("Receiving '%s' (%lu bytes) from %s",
         filename, file_size, inet_ntoa(sender_addr.sin_addr));

    char recv_fname[128];
    snprintf(recv_fname, sizeof(recv_fname)-1, "recv_%s", metadata.data);
    FILE *fp = fopen(recv_fname, "wb");
    if(!fp) err("Could not create '%s'", recv_fname);

    packet_t ack = {0};
    ack.type = T_ACK;
    ack.seq_num = 0;
    snprintf((char*)ack.data, DATA_SIZE, "%s|%lu", filename, file_size);

    if(sendto(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&sender_addr, addrlen) < 0) {
        err("sendto(ack) failed");
    }
    sccs("Handshake successful. Waiting for data");

    uint32_t expected_seq_num = 1;
    while(1) {
        packet_t recv_body = {0};
        received = recvfrom(sockfd, &recv_body, sizeof(recv_body), 0, (struct sockaddr*)&sender_addr, &addrlen);
        if(received < 0) err("recvfrom(expected_seq_num=%d) failed", expected_seq_num);

        uint32_t incoming_seq = ntohl(recv_body.seq_num);

        if(incoming_seq != expected_seq_num) {
            warn("Expected seq_num=%d", expected_seq_num);
            warn("Received seq_num=%d", recv_body.seq_num);
            // For now, I'll ignore
            //
            // TODO: Handle wrong seq_num
            // request for retransmission
        }
        expected_seq_num = incoming_seq + 1;

        ssize_t header_size = sizeof(uint8_t) + sizeof(uint32_t);
        ssize_t payload_size = received - header_size;

        if(payload_size > 0) {
            fwrite(recv_body.data, 1, payload_size, fp);
        }

        if(recv_body.type == T_EOF) {
            sccs("File received successfuly");
            break;
        }
    }
    fclose(fp);
    close(sockfd);

    return 0;
}
