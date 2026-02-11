#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
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
                strncpy(filename, argv[i+1], strlen(argv[i+1]));
                i++;
            }
            else if ((!strcmp("-i", argv[i]) || !strcmp("--ip", argv[i])) && i+1 < argc) {
                strncpy(receiver_ip, argv[i+1], strlen(argv[i+1]));
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
    char metadata[135]; // filename + "|" + unsigned long = 135
    snprintf(metadata, 127, "%s|%lu", filename, filesize);
    ssize_t bytes_sent;
    if((bytes_sent=send(network_socket, metadata, strlen(metadata), 0)) < 0) {
        err("Failed to send data");
    }
    success("MetaData sent successfully");
    sleep(1);

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
