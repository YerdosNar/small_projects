#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <poll.h>

// Custom logger
#include "logger.h"

#define LOCAL_PORT 9998
#define MODE_DIRECT 0
#define MODE_PUNCH  1

// Hole punch connect for receiver - connects to VPS and waits for peer
int punch_hole_receive(char *vps_ip, int vps_port) {
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

    close(sock);

    // TCP hole punching: simultaneous open
    // We try both listening AND connecting in parallel
    info("Initiating hole punching (receiver side)");

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

    while(tries < 20 && result_sock < 0) {
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
    }

    return result_sock;
}

int direct_receive(int port) {
    info("Creating server socket");
    int server_socket;
    if((server_socket=socket(AF_INET, SOCK_STREAM, 0)) < 0) { err("Socket creation failed");}
    success("Socket created successfully");

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    info("Bind started");
    struct sockaddr_in server_address = {
        .sin_family         = AF_INET,
        .sin_port           = htons(port),
        .sin_addr.s_addr    = INADDR_ANY
    };
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        err("Bind failed");
    }
    success("Successfully bind");

    if(listen(server_socket, 5) < 0) { err("Listen failed");}
    success("Server is listening on port %d...", port);

    int client_socket;
    if ((client_socket=accept(server_socket, NULL, NULL)) < 0) { err("Accept failed"); }

    close(server_socket);
    return client_socket;
}

void receive_file(int client_socket) {
    success("Connection established with client!");

    // Receive metadat of the file
    char metadata[256]; // filename + "|" + unsigned long
    ssize_t metadata_bytes;
    if((metadata_bytes=recv(client_socket, metadata, sizeof(metadata), MSG_WAITALL)) < 0) { err("Receive failed");}
    if(metadata_bytes != sizeof(metadata)) {err("Incomplete metadata");}

    success("Received metadata: %s", metadata);

    info("Parse filename");
    char *filename = strtok(metadata, "|");
    if(!filename) {err("Failed to parse filename");}
    success("Filename: %s", filename);
    info("Parse filesize");
    char *size_str = strtok(NULL, "|");
    if(!size_str) {err("Failed to parse filesize");}
    success("Filesize: %s", size_str);

    unsigned long filesize = strtoul(size_str, NULL, 10);
    info("Receiving: %s (%lu bytes)", filename, filesize);

    char output_fn[256];
    snprintf(output_fn, sizeof(output_fn), "recv_%s", filename);
    FILE *fp = fopen(output_fn, "wb");
    if(!fp) { err("Could not open %s", output_fn); }

    char buffer[1024];
    unsigned long total_received = 0;
    int seq = 0;
    ssize_t bytes_received;
    while((bytes_received=recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, bytes_received, 1, fp);
        total_received += bytes_received;
        float percentage = (float)total_received * 100.0 / filesize;
        printf("\r[+] Seq: %d, Recv: %lu/%lu -> %.2f", seq++, total_received, filesize, percentage);
        fflush(stdout);

        if(total_received >= filesize) break;
    }

    printf("\n");
    info("Closing sockets");
    close(client_socket);
    fclose(fp);
}

void print_usage(char *prog) {
    printf("Usage: %s [OPTIONS]\n", prog);
    printf("Options:\n");
    printf("  -m, --mode <direct|punch>  Connection mode (default: direct)\n");
    printf("  -p, --port <port>          Listen port for direct mode (default: 8080)\n");
    printf("  -v, --vps <ip>             VPS IP for hole punch mode\n");
    printf("  -V, --vps-port <port>      VPS port (default: 8888)\n");
    printf("\nExamples:\n");
    printf("  %s -m direct -p 8080\n", prog);
    printf("  %s -m punch -v 1.2.3.4\n", prog);
}

int main(int argc, char **argv) {
    int mode = MODE_DIRECT;
    int port = 8080;
    int vps_port = 8888;
    char vps_ip[16] = {0};

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

    int client_socket;

    if(mode == MODE_PUNCH) {
        if(strlen(vps_ip) == 0) {
            printf("VPS IP: ");
            fgets(vps_ip, sizeof(vps_ip), stdin);
            vps_ip[strlen(vps_ip)-1] = '\0';
        }
        info("Mode: HOLE PUNCH via VPS %s:%d", vps_ip, vps_port);
        client_socket = punch_hole_receive(vps_ip, vps_port);
        if(client_socket < 0) {
            err("Hole punch failed");
        }
    }
    else {
        info("Mode: DIRECT on port %d", port);
        client_socket = direct_receive(port);
    }

    receive_file(client_socket);
    close(client_socket);
    success("Goodbye!");

    return 0;
}
