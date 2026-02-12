#include <asm-generic/errno.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

// Custom logging
#include "logger.h"

int punch_hole_connect(char *vps_ip, int vps_port) {
    int sock;
    int opt = 1;

    struct sockaddr_in local_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9999),
        .sin_addr.s_addr = INADDR_ANY
    };

    info("Connecting to VPS to register...");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(bind(sock, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0) {
        err("Bind failed");
    }

    struct sockaddr_in vps_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(vps_port),
    };
    inet_pton(AF_INET, vps_ip, &vps_addr.sin_addr);

    if(connect(sock, (struct sockaddr *) &vps_addr, sizeof(vps_addr)) < 0) {
        err("Failed to connect to VPS");
    }
    success("Connected to VPS");

    info("Receiving peer info");
    struct sockaddr_in peer_addr;
    if(recv(sock, &peer_addr, sizeof(peer_addr), 0) <= 0) {
        err("Failed to receive peer info");
    }
    success("VPS says Peer is at %s:%d", inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));

    // Close VPS
    close(sock);

    info("Initiating hole punching");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // BIND to the same local port(9999)
    // Re-opens the exact same mapping router just created
    if(bind(sock, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0) {
        err("Bind failed");
    }

    int tries = 0;
    while(tries < 10) {
        info("\rAttempt punch: %d", tries);

        int res;
        if((res=connect(sock, (struct sockaddr*)&peer_addr, sizeof(peer_addr))) == 0) {
            success("HOLE PUNCH SUCCESSFUL! Connect to peer");
            return sock;
        }

        if(errno == ECONNREFUSED || errno == ETIMEDOUT || errno == EINPROGRESS) {
            usleep(500000);
            tries++;
        }
        else {
            err("Fatal during punch");
            return -1;
        }
    }

    warn("Hole punch timed out...");
    return -1;
}

int main(int argc, char **argv) {
    char receiver_ip[15];
    char filename[127];
    int port;
    if (argc != 7) {
        printf("Receiver's IP: ");
        fgets(receiver_ip, sizeof(receiver_ip), stdin);
        receiver_ip[strlen(receiver_ip)-1] = '\0';

        printf("Filename to send: ");
        fgets(filename, sizeof(filename), stdin);
        filename[strlen(filename)-1] = '\0';

        printf("Port number: ");
        scanf("%d", &port);
    }
    else {
        for(int i = 1; i < argc; i++) {
            if ((!strcmp("-p", argv[i]) || !strcmp("--port", argv[i])) && i+1 < argc) {
                port = atoi(argv[i+1]);
                i++;
            }
            else if ((!strcmp("-f", argv[i]) || !strcmp("--filename", argv[i])) && i+1 < argc) {
                strcpy(filename, argv[i+1]);
                i++;
            }
            else if ((!strcmp("-i", argv[i]) || !strcmp("--ip", argv[i])) && i+1 < argc) {
                strcpy(receiver_ip, argv[i+1]);
                i++;
            }
        }
    }

    int network_socket;
    info("Creating socket");
    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err("Socket creation failed");
    }
    success("Socket created succesfully!");

    struct sockaddr_in server_address = {
        .sin_family      = AF_INET,     // IPv4
        .sin_port        = htons(port), // Port number Host-to-Network
    };
    inet_pton(AF_INET, receiver_ip, &server_address.sin_addr);

    info("Connecting to remote socket");
    int con_status;
    if ((con_status=connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address))) == -1) {
        err("Connection to remote socket failed");
    }
    success("Connected to the server successfully");

    // Starting sending metadata FILENAME & FILESIZE;
    // Reading FILESIZE
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        err("Could not open: %s", filename);
    }
    fseek(fp, 0, SEEK_END);
    unsigned long filesize = ftell(fp);
    rewind(fp);

    info("Sending metadata");
    char metadata[256] = {0}; // filename + "|" + unsigned long = 135
    snprintf(metadata, sizeof(metadata), "%s|%lu", filename, filesize);
    ssize_t bytes_sent;
    if((bytes_sent=send(network_socket, metadata, sizeof(metadata), 0)) < 0) { err("Failed to send data");}
    success("MetaData sent successfully");

    int seq = 1;
    unsigned long total_sent = 0;
    char buffer[1024];
    size_t bytes_read;
    while((bytes_read=fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if((bytes_sent=send(network_socket, buffer, bytes_read, 0)) < 0) {
            err("Failed to send data");
        }
        total_sent += bytes_sent;
        float percentage = (float)total_sent * 100.0 / filesize;
        printf("\r[+] Seq: %d | Sent: %lu/%lu -> %.2f%%", seq++, total_sent, filesize, percentage);
        fflush(stdout);
    }
    printf("\n");

    if(feof(fp)) {
        info("EOF reached.");
    }
    else if(ferror(fp)) {
        err("Error occured");
    }

    info("Closing socket");
    fclose(fp);
    close(network_socket);
    success("Goodbye!");

    return 0;
}
