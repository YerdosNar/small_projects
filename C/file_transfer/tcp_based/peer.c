#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "logger.h"
#include "protocol.h"

// ============================================================================
// Global State
// ============================================================================

static int g_main_socket = -1;
static int g_running = 1;
static pthread_mutex_t g_socket_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_transfer_mutex = PTHREAD_MUTEX_INITIALIZER;

static file_transfer_t g_transfers[MAX_TRANSFERS];
static uint32_t g_next_transfer_id = 1;

static struct sockaddr_in g_peer_addr;  // For file transfer connections
static int g_is_initiator = 0;          // Who initiated the main connection

// ============================================================================
// Utility Functions
// ============================================================================

void cleanup(int sig) {
    (void)sig;
    g_running = 0;
    if (g_main_socket >= 0) {
        shutdown(g_main_socket, SHUT_RDWR);
        close(g_main_socket);
    }
    info("\nShutting down...");
    exit(0);
}

int send_all(int sock, const void *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(sock, (char*)buf + sent, len - sent, 0);
        if (n <= 0) return -1;
        sent += n;
    }
    return 0;
}

int recv_all(int sock, void *buf, size_t len) {
    size_t received = 0;
    while (received < len) {
        ssize_t n = recv(sock, (char*)buf + received, len - received, 0);
        if (n <= 0) return -1;
        received += n;
    }
    return 0;
}

// ============================================================================
// Protocol Functions
// ============================================================================

int send_message(int sock, uint8_t type, const void *payload, uint32_t len) {
    msg_header_t header = {
        .type = type,
        .length = htonl(len)
    };
    
    pthread_mutex_lock(&g_socket_mutex);
    int ret = send_all(sock, &header, sizeof(header));
    if (ret == 0 && len > 0) {
        ret = send_all(sock, payload, len);
    }
    pthread_mutex_unlock(&g_socket_mutex);
    
    return ret;
}

int recv_header(int sock, msg_header_t *header) {
    if (recv_all(sock, header, sizeof(*header)) < 0) return -1;
    header->length = ntohl(header->length);
    return 0;
}

// ============================================================================
// Transfer Management
// ============================================================================

file_transfer_t* find_transfer(uint32_t id) {
    for (int i = 0; i < MAX_TRANSFERS; i++) {
        if (g_transfers[i].active && g_transfers[i].transfer_id == id) {
            return &g_transfers[i];
        }
    }
    return NULL;
}

file_transfer_t* alloc_transfer() {
    for (int i = 0; i < MAX_TRANSFERS; i++) {
        if (!g_transfers[i].active) {
            memset(&g_transfers[i], 0, sizeof(file_transfer_t));
            g_transfers[i].active = 1;
            g_transfers[i].data_socket = -1;
            return &g_transfers[i];
        }
    }
    return NULL;
}

void free_transfer(file_transfer_t *t) {
    if (t->fp) fclose(t->fp);
    if (t->data_socket >= 0) close(t->data_socket);
    t->active = 0;
}

// ============================================================================
// Connection Establishment (Same as before)
// ============================================================================

int punch_hole_connect(char *vps_ip, int vps_port, int local_port) {
    int sock;
    int opt = 1;

    struct sockaddr_in local_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(local_port),
        .sin_addr.s_addr = INADDR_ANY
    };

    info("Connecting to VPS to register...");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        err("Bind failed");
    }

    struct sockaddr_in vps_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(vps_port),
    };
    inet_pton(AF_INET, vps_ip, &vps_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&vps_addr, sizeof(vps_addr)) < 0) {
        err("Failed to connect to VPS");
    }
    success("Connected to VPS");

    info("Receiving peer info");
    if (recv(sock, &g_peer_addr, sizeof(g_peer_addr), 0) <= 0) {
        err("Failed to receive peer info");
    }
    success("VPS says Peer is at %s:%d", inet_ntoa(g_peer_addr.sin_addr), ntohs(g_peer_addr.sin_port));

    close(sock);

    // TCP hole punching with simultaneous open
    info("Initiating hole punching...");

    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(listen_sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        err("Listen bind failed");
    }
    listen(listen_sock, 1);

    int conn_sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(conn_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(conn_sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    if (bind(conn_sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        err("Connect bind failed");
    }

    fcntl(listen_sock, F_SETFL, O_NONBLOCK);
    fcntl(conn_sock, F_SETFL, O_NONBLOCK);

    int tries = 0;
    int result_sock = -1;

    while (tries < 20 && result_sock < 0) {
        info("Attempt %d: trying listen + connect...", tries);

        struct sockaddr_in accepted_addr;
        socklen_t addr_len = sizeof(accepted_addr);
        int accepted = accept(listen_sock, (struct sockaddr *)&accepted_addr, &addr_len);
        if (accepted >= 0) {
            success("HOLE PUNCH: Accepted connection from peer!");
            result_sock = accepted;
            g_is_initiator = 0;
            break;
        }

        int res = connect(conn_sock, (struct sockaddr *)&g_peer_addr, sizeof(g_peer_addr));
        if (res == 0 || (res < 0 && errno == EISCONN)) {
            success("HOLE PUNCH: Connected to peer!");
            result_sock = conn_sock;
            conn_sock = -1;
            g_is_initiator = 1;
            break;
        }

        if (errno == EINPROGRESS || errno == EALREADY) {
            struct pollfd pfd = {.fd = conn_sock, .events = POLLOUT};
            if (poll(&pfd, 1, 100) > 0) {
                int error = 0;
                socklen_t len = sizeof(error);
                getsockopt(conn_sock, SOL_SOCKET, SO_ERROR, &error, &len);
                if (error == 0) {
                    success("HOLE PUNCH: Connected to peer!");
                    result_sock = conn_sock;
                    conn_sock = -1;
                    g_is_initiator = 1;
                    break;
                }
            }
        }

        usleep(300000);
        tries++;
    }

    close(listen_sock);
    if (conn_sock >= 0) close(conn_sock);

    if (result_sock >= 0) {
        fcntl(result_sock, F_SETFL, fcntl(result_sock, F_GETFL) & ~O_NONBLOCK);
    }

    return result_sock;
}

int direct_connect(char *ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        err("Socket creation failed");
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };
    inet_pton(AF_INET, ip, &addr.sin_addr);
    memcpy(&g_peer_addr, &addr, sizeof(addr));

    info("Connecting to %s:%d...", ip, port);
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        err("Connection failed");
    }
    success("Connected!");
    g_is_initiator = 1;

    return sock;
}

int direct_listen(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        err("Bind failed");
    }
    listen(sock, 1);
    info("Listening on port %d...", port);

    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    int client = accept(sock, (struct sockaddr *)&peer, &len);
    if (client < 0) {
        err("Accept failed");
    }
    
    memcpy(&g_peer_addr, &peer, sizeof(peer));
    success("Connection from %s:%d", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
    close(sock);
    g_is_initiator = 0;

    return client;
}

// ============================================================================
// File Transfer Threads
// ============================================================================

void* file_send_thread(void *arg) {
    file_transfer_t *t = (file_transfer_t *)arg;
    
    // Create a new socket for file data
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Bind to an ephemeral port
    struct sockaddr_in local = {
        .sin_family = AF_INET,
        .sin_port = 0,
        .sin_addr.s_addr = INADDR_ANY
    };
    bind(sock, (struct sockaddr *)&local, sizeof(local));
    listen(sock, 1);
    
    // Get the assigned port
    socklen_t len = sizeof(local);
    getsockname(sock, (struct sockaddr *)&local, &len);
    t->data_port = ntohs(local.sin_port);
    
    // Send port info to peer
    struct {
        uint32_t transfer_id;
        uint16_t port;
    } __attribute__((packed)) port_msg = {
        .transfer_id = htonl(t->transfer_id),
        .port = htons(t->data_port)
    };
    send_message(g_main_socket, MSG_FILE_PORT, &port_msg, sizeof(port_msg));
    
    info("[Transfer %u] Waiting for receiver on port %u...", t->transfer_id, t->data_port);
    
    // Wait for connection with timeout
    struct pollfd pfd = {.fd = sock, .events = POLLIN};
    if (poll(&pfd, 1, 30000) <= 0) {
        warn("[Transfer %u] Timeout waiting for receiver", t->transfer_id);
        close(sock);
        pthread_mutex_lock(&g_transfer_mutex);
        free_transfer(t);
        pthread_mutex_unlock(&g_transfer_mutex);
        return NULL;
    }
    
    int data_sock = accept(sock, NULL, NULL);
    close(sock);
    
    if (data_sock < 0) {
        warn("[Transfer %u] Accept failed", t->transfer_id);
        pthread_mutex_lock(&g_transfer_mutex);
        free_transfer(t);
        pthread_mutex_unlock(&g_transfer_mutex);
        return NULL;
    }
    
    t->data_socket = data_sock;
    info("[Transfer %u] Sending file: %s", t->transfer_id, t->filename);
    
    // Send file data
    char buffer[CHUNK_SIZE];
    size_t bytes_read;
    
    while (g_running && (bytes_read = fread(buffer, 1, sizeof(buffer), t->fp)) > 0) {
        if (send_all(data_sock, buffer, bytes_read) < 0) {
            warn("[Transfer %u] Send error", t->transfer_id);
            break;
        }
        t->transferred += bytes_read;
        
        float pct = (float)t->transferred * 100.0 / t->filesize;
        printf("\r[Transfer %u] Sent: %.1f%%", t->transfer_id, pct);
        fflush(stdout);
    }
    printf("\n");
    
    if (t->transferred >= t->filesize) {
        success("[Transfer %u] File sent successfully!", t->transfer_id);
    }
    
    pthread_mutex_lock(&g_transfer_mutex);
    free_transfer(t);
    pthread_mutex_unlock(&g_transfer_mutex);
    
    return NULL;
}

void* file_recv_thread(void *arg) {
    file_transfer_t *t = (file_transfer_t *)arg;
    
    // Wait for port info (will be set by receiver thread)
    int timeout = 100;  // 10 seconds
    while (t->data_port == 0 && timeout-- > 0 && g_running) {
        usleep(100000);
    }
    
    if (t->data_port == 0) {
        warn("[Transfer %u] Timeout waiting for port info", t->transfer_id);
        pthread_mutex_lock(&g_transfer_mutex);
        free_transfer(t);
        pthread_mutex_unlock(&g_transfer_mutex);
        return NULL;
    }
    
    // Connect to sender's data port
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = g_peer_addr;
    addr.sin_port = htons(t->data_port);
    
    info("[Transfer %u] Connecting to sender port %u...", t->transfer_id, t->data_port);
    
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        warn("[Transfer %u] Connect failed: %s", t->transfer_id, strerror(errno));
        close(sock);
        pthread_mutex_lock(&g_transfer_mutex);
        free_transfer(t);
        pthread_mutex_unlock(&g_transfer_mutex);
        return NULL;
    }
    
    t->data_socket = sock;
    info("[Transfer %u] Receiving file: %s", t->transfer_id, t->filename);
    
    // Receive file data
    char buffer[CHUNK_SIZE];
    ssize_t bytes_recv;
    
    while (g_running && t->transferred < t->filesize) {
        size_t to_recv = t->filesize - t->transferred;
        if (to_recv > sizeof(buffer)) to_recv = sizeof(buffer);
        
        bytes_recv = recv(sock, buffer, to_recv, 0);
        if (bytes_recv <= 0) {
            warn("[Transfer %u] Recv error", t->transfer_id);
            break;
        }
        
        fwrite(buffer, 1, bytes_recv, t->fp);
        t->transferred += bytes_recv;
        
        float pct = (float)t->transferred * 100.0 / t->filesize;
        printf("\r[Transfer %u] Received: %.1f%%", t->transfer_id, pct);
        fflush(stdout);
    }
    printf("\n");
    
    if (t->transferred >= t->filesize) {
        success("[Transfer %u] File received: recv_%s", t->transfer_id, t->filename);
    }
    
    pthread_mutex_lock(&g_transfer_mutex);
    free_transfer(t);
    pthread_mutex_unlock(&g_transfer_mutex);
    
    return NULL;
}

// ============================================================================
// Main Sender Thread (reads stdin, sends messages)
// ============================================================================

void* sender_thread(void *arg) {
    (void)arg;
    char input[MAX_MSG_LEN];
    
    printf("\n");
    printf("========================================\n");
    printf("  P2P Messenger - Commands:\n");
    printf("  /send <filename>  - Send a file\n");
    printf("  /accept <id>      - Accept file transfer\n");
    printf("  /reject <id>      - Reject file transfer\n");
    printf("  /transfers        - List active transfers\n");
    printf("  /quit             - Exit\n");
    printf("========================================\n");
    printf("\n");
    
    while (g_running) {
        printf("You: ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        // Remove newline
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') {
            input[--len] = '\0';
        }
        
        if (len == 0) continue;
        
        // Handle commands
        if (input[0] == '/') {
            if (strncmp(input, "/send ", 6) == 0) {
                char *filename = input + 6;
                FILE *fp = fopen(filename, "rb");
                if (!fp) {
                    warn("Cannot open file: %s", filename);
                    continue;
                }
                
                fseek(fp, 0, SEEK_END);
                uint64_t size = ftell(fp);
                rewind(fp);
                
                pthread_mutex_lock(&g_transfer_mutex);
                file_transfer_t *t = alloc_transfer();
                if (!t) {
                    warn("Too many active transfers");
                    fclose(fp);
                    pthread_mutex_unlock(&g_transfer_mutex);
                    continue;
                }
                
                t->transfer_id = g_next_transfer_id++;
                strncpy(t->filename, filename, MAX_FILENAME - 1);
                t->filesize = size;
                t->fp = fp;
                t->is_sender = 1;
                pthread_mutex_unlock(&g_transfer_mutex);
                
                // Send file request
                file_request_t req = {
                    .filesize = size,
                    .transfer_id = htonl(t->transfer_id)
                };
                
                // Extract basename
                char *base = strrchr(filename, '/');
                base = base ? base + 1 : filename;
                strncpy(req.filename, base, MAX_FILENAME - 1);
                
                info("Requesting to send: %s (%lu bytes) [ID: %u]", 
                     base, (unsigned long)size, t->transfer_id);
                send_message(g_main_socket, MSG_FILE_REQ, &req, sizeof(req));
            }
            else if (strncmp(input, "/accept ", 8) == 0) {
                uint32_t id = atoi(input + 8);
                
                pthread_mutex_lock(&g_transfer_mutex);
                file_transfer_t *t = find_transfer(id);
                if (!t || t->is_sender) {
                    warn("Invalid transfer ID: %u", id);
                    pthread_mutex_unlock(&g_transfer_mutex);
                    continue;
                }
                
                // Create output file
                char outname[MAX_FILENAME + 8];
                snprintf(outname, sizeof(outname), "recv_%s", t->filename);
                t->fp = fopen(outname, "wb");
                if (!t->fp) {
                    warn("Cannot create file: %s", outname);
                    free_transfer(t);
                    pthread_mutex_unlock(&g_transfer_mutex);
                    continue;
                }
                pthread_mutex_unlock(&g_transfer_mutex);
                
                // Send accept
                uint32_t net_id = htonl(id);
                send_message(g_main_socket, MSG_FILE_ACCEPT, &net_id, sizeof(net_id));
                
                // Start receiver thread
                pthread_t tid;
                pthread_create(&tid, NULL, file_recv_thread, t);
                pthread_detach(tid);
            }
            else if (strncmp(input, "/reject ", 8) == 0) {
                uint32_t id = atoi(input + 8);
                
                pthread_mutex_lock(&g_transfer_mutex);
                file_transfer_t *t = find_transfer(id);
                if (t && !t->is_sender) {
                    free_transfer(t);
                }
                pthread_mutex_unlock(&g_transfer_mutex);
                
                uint32_t net_id = htonl(id);
                send_message(g_main_socket, MSG_FILE_REJECT, &net_id, sizeof(net_id));
                info("Rejected transfer %u", id);
            }
            else if (strcmp(input, "/transfers") == 0) {
                printf("\nActive transfers:\n");
                pthread_mutex_lock(&g_transfer_mutex);
                int count = 0;
                for (int i = 0; i < MAX_TRANSFERS; i++) {
                    if (g_transfers[i].active) {
                        file_transfer_t *t = &g_transfers[i];
                        float pct = t->filesize > 0 ? 
                            (float)t->transferred * 100.0 / t->filesize : 0;
                        printf("  [%u] %s %s: %.1f%%\n", 
                               t->transfer_id,
                               t->is_sender ? "SEND" : "RECV",
                               t->filename, pct);
                        count++;
                    }
                }
                pthread_mutex_unlock(&g_transfer_mutex);
                if (count == 0) printf("  (none)\n");
                printf("\n");
            }
            else if (strcmp(input, "/quit") == 0) {
                g_running = 0;
                break;
            }
            else {
                warn("Unknown command: %s", input);
            }
        }
        else {
            // Send text message
            if (send_message(g_main_socket, MSG_TEXT, input, len) < 0) {
                warn("Failed to send message");
                break;
            }
        }
    }
    
    return NULL;
}

// ============================================================================
// Main Receiver Thread (receives messages)
// ============================================================================

void* receiver_thread(void *arg) {
    (void)arg;
    msg_header_t header;
    char buffer[MAX_MSG_LEN + 1];
    
    while (g_running) {
        if (recv_header(g_main_socket, &header) < 0) {
            if (g_running) warn("Connection lost");
            break;
        }
        
        if (header.length > MAX_MSG_LEN) {
            warn("Message too large");
            break;
        }
        
        if (header.length > 0) {
            if (recv_all(g_main_socket, buffer, header.length) < 0) {
                warn("Failed to receive payload");
                break;
            }
        }
        
        switch (header.type) {
            case MSG_TEXT:
                buffer[header.length] = '\0';
                printf("\r\033[K");  // Clear current line
                printf("Peer: %s\n", buffer);
                printf("You: ");
                fflush(stdout);
                break;
                
            case MSG_FILE_REQ: {
                file_request_t *req = (file_request_t *)buffer;
                uint32_t id = ntohl(req->transfer_id);
                
                pthread_mutex_lock(&g_transfer_mutex);
                file_transfer_t *t = alloc_transfer();
                if (t) {
                    t->transfer_id = id;
                    strncpy(t->filename, req->filename, MAX_FILENAME - 1);
                    t->filesize = req->filesize;
                    t->is_sender = 0;
                }
                pthread_mutex_unlock(&g_transfer_mutex);
                
                printf("\r\033[K");
                printf(">>> Incoming file: %s (%lu bytes) [ID: %u]\n",
                       req->filename, (unsigned long)req->filesize, id);
                printf(">>> Type /accept %u or /reject %u\n", id, id);
                printf("You: ");
                fflush(stdout);
                break;
            }
            
            case MSG_FILE_ACCEPT: {
                uint32_t id = ntohl(*(uint32_t *)buffer);
                
                pthread_mutex_lock(&g_transfer_mutex);
                file_transfer_t *t = find_transfer(id);
                if (t && t->is_sender) {
                    pthread_mutex_unlock(&g_transfer_mutex);
                    
                    printf("\r\033[K");
                    info("Transfer %u accepted, starting...", id);
                    printf("You: ");
                    fflush(stdout);
                    
                    pthread_t tid;
                    pthread_create(&tid, NULL, file_send_thread, t);
                    pthread_detach(tid);
                } else {
                    pthread_mutex_unlock(&g_transfer_mutex);
                }
                break;
            }
            
            case MSG_FILE_REJECT: {
                uint32_t id = ntohl(*(uint32_t *)buffer);
                
                pthread_mutex_lock(&g_transfer_mutex);
                file_transfer_t *t = find_transfer(id);
                if (t) {
                    free_transfer(t);
                }
                pthread_mutex_unlock(&g_transfer_mutex);
                
                printf("\r\033[K");
                warn("Transfer %u rejected by peer", id);
                printf("You: ");
                fflush(stdout);
                break;
            }
            
            case MSG_FILE_PORT: {
                struct {
                    uint32_t transfer_id;
                    uint16_t port;
                } __attribute__((packed)) *port_msg = (void *)buffer;
                
                uint32_t id = ntohl(port_msg->transfer_id);
                uint16_t port = ntohs(port_msg->port);
                
                pthread_mutex_lock(&g_transfer_mutex);
                file_transfer_t *t = find_transfer(id);
                if (t && !t->is_sender) {
                    t->data_port = port;
                }
                pthread_mutex_unlock(&g_transfer_mutex);
                break;
            }
            
            default:
                warn("Unknown message type: %02x", header.type);
        }
    }
    
    g_running = 0;
    return NULL;
}

// ============================================================================
// Main
// ============================================================================

void print_usage(char *prog) {
    printf("Usage: %s [OPTIONS]\n\n", prog);
    printf("Connection modes:\n");
    printf("  -m, --mode <connect|listen|punch>  Connection mode\n");
    printf("  -i, --ip <ip>                      Peer IP (connect mode)\n");
    printf("  -p, --port <port>                  Port (default: 8080)\n");
    printf("  -v, --vps <ip>                     VPS IP (punch mode)\n");
    printf("  -V, --vps-port <port>              VPS port (default: 8888)\n");
    printf("\nExamples:\n");
    printf("  %s -m listen -p 8080          # Wait for connection\n", prog);
    printf("  %s -m connect -i 1.2.3.4 -p 8080  # Connect to peer\n", prog);
    printf("  %s -m punch -v 5.6.7.8        # Use VPS for NAT traversal\n", prog);
}

int main(int argc, char **argv) {
    int mode = -1;  // 0=connect, 1=listen, 2=punch
    char peer_ip[16] = {0};
    char vps_ip[16] = {0};
    int port = 8080;
    int vps_port = 8888;
    int local_port = 9999;
    
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        if ((!strcmp("-m", argv[i]) || !strcmp("--mode", argv[i])) && i+1 < argc) {
            if (!strcmp("connect", argv[i+1])) mode = 0;
            else if (!strcmp("listen", argv[i+1])) mode = 1;
            else if (!strcmp("punch", argv[i+1])) mode = 2;
            i++;
        }
        else if ((!strcmp("-i", argv[i]) || !strcmp("--ip", argv[i])) && i+1 < argc) {
            strncpy(peer_ip, argv[i+1], sizeof(peer_ip) - 1);
            i++;
        }
        else if ((!strcmp("-p", argv[i]) || !strcmp("--port", argv[i])) && i+1 < argc) {
            port = atoi(argv[i+1]);
            i++;
        }
        else if ((!strcmp("-v", argv[i]) || !strcmp("--vps", argv[i])) && i+1 < argc) {
            strncpy(vps_ip, argv[i+1], sizeof(vps_ip) - 1);
            i++;
        }
        else if ((!strcmp("-V", argv[i]) || !strcmp("--vps-port", argv[i])) && i+1 < argc) {
            vps_port = atoi(argv[i+1]);
            i++;
        }
        else if ((!strcmp("-l", argv[i]) || !strcmp("--local-port", argv[i])) && i+1 < argc) {
            local_port = atoi(argv[i+1]);
            i++;
        }
        else if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i])) {
            print_usage(argv[0]);
            return 0;
        }
    }
    
    // Interactive mode selection if not specified
    if (mode < 0) {
        printf("Connection mode:\n");
        printf("  1. Connect to peer\n");
        printf("  2. Listen for peer\n");
        printf("  3. Hole punch (NAT traversal)\n");
        printf("Select [1-3]: ");
        
        char choice[8];
        fgets(choice, sizeof(choice), stdin);
        mode = atoi(choice) - 1;
        
        if (mode == 0 && strlen(peer_ip) == 0) {
            printf("Peer IP: ");
            fgets(peer_ip, sizeof(peer_ip), stdin);
            peer_ip[strcspn(peer_ip, "\n")] = '\0';
        }
        else if (mode == 2 && strlen(vps_ip) == 0) {
            printf("VPS IP: ");
            fgets(vps_ip, sizeof(vps_ip), stdin);
            vps_ip[strcspn(vps_ip, "\n")] = '\0';
        }
    }
    
    // Setup signal handler
    signal(SIGINT, cleanup);
    signal(SIGPIPE, SIG_IGN);
    
    // Establish connection
    switch (mode) {
        case 0:  // Connect
            if (strlen(peer_ip) == 0) {
                err("Peer IP required for connect mode");
            }
            g_main_socket = direct_connect(peer_ip, port);
            break;
            
        case 1:  // Listen
            g_main_socket = direct_listen(port);
            break;
            
        case 2:  // Punch
            if (strlen(vps_ip) == 0) {
                err("VPS IP required for punch mode");
            }
            g_main_socket = punch_hole_connect(vps_ip, vps_port, local_port);
            break;
            
        default:
            err("Invalid mode");
    }
    
    if (g_main_socket < 0) {
        err("Connection failed");
    }
    
    success("P2P connection established!");
    
    // Start sender and receiver threads
    pthread_t sender_tid, receiver_tid;
    
    pthread_create(&receiver_tid, NULL, receiver_thread, NULL);
    pthread_create(&sender_tid, NULL, sender_thread, NULL);
    
    // Wait for threads
    pthread_join(sender_tid, NULL);
    g_running = 0;
    
    // Cleanup
    shutdown(g_main_socket, SHUT_RDWR);
    pthread_join(receiver_tid, NULL);
    close(g_main_socket);
    
    success("Goodbye!");
    return 0;
}
