#include <asm-generic/errno.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <poll.h>

// To draw progress bar
#include <sys/ioctl.h>

// Custom logging
#include "logger.h"

#define BUFFER_SIZE 1024 * 64 // Let's make 64KB buffer
#define LOCAL_PORT 9999
#define MODE_DIRECT 0
#define MODE_PUNCH  1

void print_progress_bar(int percentage) {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    int cols = w.ws_col;
    int bar_width = cols - 8; // cols - [] - 100%
    int fill_bar = percentage * 100 / bar_width;
    printf("\r[");
    for(int i = 0; i < bar_width; i++) {
        if(i < fill_bar) {
            printf(B_GRN);
        }
        else {
            printf(" ");
        }
    }
    printf("] %d%%", percentage);
    fflush(stdout);
}

int punch_hole_connect(char *vps_ip, int vps_port) {
    int sock;
    int opt = 1;

    struct sockaddr_in local_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(LOCAL_PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    info("Connecting to VPS to register...");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
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

    // TCP hole punching: simultaneous open
    // We try both listening AND connecting in parallel
    info("Initiating hole punching (sender side)");

    // Create listening socket
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if(bind(listen_sock, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0) {
        err("Listen bind failed");
    }
    listen(listen_sock, 1);

    // Create connecting socket
    int conn_sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(conn_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(conn_sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if(bind(conn_sock, (struct sockaddr *) &local_addr, sizeof(local_addr)) < 0) {
        err("Connect bind failed");
    }

    // Set non-blocking
    fcntl(listen_sock, F_SETFL, O_NONBLOCK);
    fcntl(conn_sock, F_SETFL, O_NONBLOCK);

    int tries = 0;
    int result_sock = -1;

    while(tries < 50 && result_sock < 0) {
        info("Attempt %d: trying listen + connect...", tries);

        // Try to accept
        struct sockaddr_in accepted_addr;
        socklen_t addr_len = sizeof(accepted_addr);
        int accepted = accept(listen_sock, (struct sockaddr *)&accepted_addr, &addr_len);
        if(accepted >= 0) {
            success("HOLE PUNCH: Accepted connection from peer!");
            result_sock = accepted;
            break;
        }

        // Try to connect
        int res = connect(conn_sock, (struct sockaddr*)&peer_addr, sizeof(peer_addr));
        if(res == 0 || (res < 0 && errno == EISCONN)) {
            success("HOLE PUNCH: Connected to peer!");
            result_sock = conn_sock;
            conn_sock = -1;  // Don't close this one
            break;
        }

        if(errno == EINPROGRESS || errno == EALREADY) {
            // Connection in progress, check with poll
            struct pollfd pfd = {.fd = conn_sock, .events = POLLOUT};
            if(poll(&pfd, 1, 100) > 0) {
                int error = 0;
                socklen_t len = sizeof(error);
                getsockopt(conn_sock, SOL_SOCKET, SO_ERROR, &error, &len);
                if(error == 0) {
                    success("HOLE PUNCH: Connected to peer!");
                    result_sock = conn_sock;
                    conn_sock = -1;
                    break;
                }
            }
        }

        usleep(300000);
        tries++;
    }

    close(listen_sock);
    if(conn_sock >= 0) close(conn_sock);

    if(result_sock >= 0) {
        // Set back to blocking
        fcntl(result_sock, F_SETFL, fcntl(result_sock, F_GETFL) & ~O_NONBLOCK);
    } else {
        warn("Hole punch timed out...");
    }

    return result_sock;
}

int direct_connect(char *receiver_ip, int port) {
    int network_socket;
    info("Creating socket");
    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err("Socket creation failed");
    }
    success("Socket created succesfully!");

    struct sockaddr_in server_address = {
        .sin_family      = AF_INET,
        .sin_port        = htons(port),
    };
    inet_pton(AF_INET, receiver_ip, &server_address.sin_addr);

    info("Connecting to remote socket");
    if (connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        err("Connection to remote socket failed");
    }
    success("Connected to the server successfully");

    return network_socket;
}

void send_file(int network_socket, char *filename) {
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

    // Extract just the filename from path
    char *base_filename = strrchr(filename, '/');
    base_filename = base_filename ? base_filename + 1 : filename;

    snprintf(metadata, sizeof(metadata), "%s|%lu", base_filename, filesize);
    ssize_t bytes_sent;
    if((bytes_sent=send(network_socket, metadata, sizeof(metadata), 0)) < 0) { err("Failed to send data");}
    success("MetaData sent successfully");

    int seq = 1;
    unsigned long total_sent = 0;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int last_percent = -1;
    while((bytes_read=fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if((bytes_sent=send(network_socket, buffer, bytes_read, 0)) < 0) {
            err("Failed to send data");
        }
        total_sent += bytes_sent;
        int curr_percentage = total_sent * 100 / filesize;
        if (curr_percentage > last_percent) {
            // printf("\r[+] Seq: %d | Sent: %lu/%lu -> %d%%", seq++, total_sent, filesize, curr_percentage);
            // fflush(stdout);
            print_progress_bar(curr_percentage);
            last_percent = curr_percentage;
        }
    }
    printf("\n");

    if(feof(fp)) {
        info("EOF reached.");
    }
    else if(ferror(fp)) {
        err("Error occured");
    }

    fclose(fp);
}

void print_usage(char *prog) {
    printf("Usage: %s [OPTIONS]\n", prog);
    printf("Options:\n");
    printf("  -m, --mode <direct|punch>  Connection mode (default: direct)\n");
    printf("  -f, --filename <file>      File to send\n");
    printf("  -i, --ip <ip>              Receiver IP (direct mode)\n");
    printf("  -p, --port <port>          Receiver port (default: 8080)\n");
    printf("  -v, --vps <ip>             VPS IP for hole punch mode\n");
    printf("  -V, --vps-port <port>      VPS port (default: 8888)\n");
    printf("\nExamples:\n");
    printf("  %s -m direct -i 192.168.1.10 -p 8080 -f myfile.txt\n", prog);
    printf("  %s -m punch -v 1.2.3.4 -f myfile.txt\n", prog);
}

int main(int argc, char **argv) {
    int mode = MODE_DIRECT;
    char receiver_ip[16] = {0};
    char vps_ip[16] = {0};
    char filename[256] = {0};
    int port = 8080;
    int vps_port = 8888;

    for(int i = 1; i < argc; i++) {
        if((!strcmp("-m", argv[i]) || !strcmp("--mode", argv[i])) && i+1 < argc) {
            if(!strcmp("punch", argv[i+1])) mode = MODE_PUNCH;
            else mode = MODE_DIRECT;
            i++;
        }
        else if((!strcmp("-p", argv[i]) || !strcmp("--port", argv[i])) && i+1 < argc) {
            port = atoi(argv[i+1]);
            i++;
        }
        else if((!strcmp("-f", argv[i]) || !strcmp("--filename", argv[i])) && i+1 < argc) {
            strcpy(filename, argv[i+1]);
            i++;
        }
        else if((!strcmp("-i", argv[i]) || !strcmp("--ip", argv[i])) && i+1 < argc) {
            strcpy(receiver_ip, argv[i+1]);
            i++;
        }
        else if((!strcmp("-v", argv[i]) || !strcmp("--vps", argv[i])) && i+1 < argc) {
            strcpy(vps_ip, argv[i+1]);
            i++;
        }
        else if((!strcmp("-V", argv[i]) || !strcmp("--vps-port", argv[i])) && i+1 < argc) {
            vps_port = atoi(argv[i+1]);
            i++;
        }
        else if(!strcmp("-h", argv[i]) || !strcmp("--help", argv[i])) {
            print_usage(argv[0]);
            return 0;
        }
    }

    // Interactive mode if arguments missing
    if(strlen(filename) == 0) {
        printf("Filename to send: ");
        fgets(filename, sizeof(filename), stdin);
        filename[strlen(filename)-1] = '\0';
    }

    int network_socket;

    if(mode == MODE_PUNCH) {
        if(strlen(vps_ip) == 0) {
            printf("VPS IP: ");
            fgets(vps_ip, sizeof(vps_ip), stdin);
            vps_ip[strlen(vps_ip)-1] = '\0';
        }
        info("Mode: HOLE PUNCH via VPS %s:%d", vps_ip, vps_port);
        network_socket = punch_hole_connect(vps_ip, vps_port);
        if(network_socket < 0) {
            err("Hole punch failed");
        }
    }
    else {
        if(strlen(receiver_ip) == 0) {
            printf("Receiver's IP: ");
            fgets(receiver_ip, sizeof(receiver_ip), stdin);
            receiver_ip[strlen(receiver_ip)-1] = '\0';
        }
        info("Mode: DIRECT to %s:%d", receiver_ip, port);
        network_socket = direct_connect(receiver_ip, port);
    }

    send_file(network_socket, filename);

    info("Closing socket");
    close(network_socket);
    success("Goodbye!");

    return 0;
}
