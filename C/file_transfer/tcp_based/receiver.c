#include <netinet/in.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define NOC "\033[0m"

void info(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    printf(BLU "[i]" NOC " ");
    vprintf(msg, args);
    printf("\n");
    va_end(args);
}
void warn(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    printf(YEL "[!]" NOC " ");
    vprintf(msg, args);
    printf("\n");
    va_end(args);
}
void success(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    printf(GRN "[✓]" NOC " ");
    vprintf(msg, args);
    printf("\n");
    va_end(args);
}
void err(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    printf(RED "[x]" NOC " ");
    vprintf(msg, args);
    printf("\n");
    va_end(args);

    exit(1);
}

int main(void) {
    info("Creating server socket");
    int server_socket;
    if((server_socket=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err("Socket creation failed");
    }
    success("Socket created successfully");

    info("Bind started");
    struct sockaddr_in server_address = {
        .sin_family         = AF_INET,
        .sin_port           = htons(8080),
        .sin_addr.s_addr    = INADDR_ANY
    };
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        err("Bind failed");
    }
    success("Successfully bind");

    if(listen(server_socket, 5) < 0) { err("Listen failed");}
    success("Server is listening on port 8080...");

    int client_socket;
    if ((client_socket=accept(server_socket, NULL, NULL)) < 0) { err("Accept failed"); }
    success("Connection established with client!");

    // Receive metadat of the file
    char metadata[1024] = {0}; // filename + "|" + unsigned long
    ssize_t bytes_received;
    if((bytes_received=recv(client_socket, metadata, sizeof(metadata)-1, 0)) < 0) {
        err("Receive failed");
    }
    metadata[bytes_received] = '\0';
    info("Received metadata: %s\n", metadata);

    printf("metadata size: %lu\n", strlen(metadata));
    for(int i = 0; i < strlen(metadata); i++) {
        printf("%d, ", metadata[i]);
    }

    char *filename = strtok(metadata, "|");
    if(!filename) {err("Failed to parse filename");}
    char *size_str = strtok(NULL, "|");
    if(!size_str) {err("Failed to parse filesize");}

    unsigned long filesize = strtoul(size_str, NULL, 10);
    info("Receiving: %s (%lu bytes)", filesize, filesize);

    char output_fn[256];
    snprintf(output_fn, sizeof(output_fn), "recv_%s", filename);
    FILE *fp = fopen(output_fn, "wb");
    if(!fp) { err("Could not open %s", output_fn); }

    char buffer[1024];
    unsigned long total_received = 0;
    int seq = 0;
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
    close(server_socket);
    fclose(fp);

    return 0;
}
