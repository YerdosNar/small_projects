#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define NOC "\033[0m"

void info(char *msg)    { printf(BLU "[i]" NOC " %s\n", msg);         }
void warn(char *msg)    { printf(YEL "[!]" NOC " %s\n", msg);         }
void success(char *msg) { printf(GRN "[✓]" NOC " %s\n", msg);         }
void err(char *msg)     { printf(RED "[x]" NOC " %s\n", msg);exit(1); }

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

    if(listen(server_socket, 5) < 0) {
        err("Listen failed");
    }
    success("Server is listening on port 8080...");

    int client_socket;
    if ((client_socket=accept(server_socket, NULL, NULL)) < 0) {
        err("Accept failed");
    }
    success("Connection established with client!");

    char server_response[256];

    ssize_t bytes_received;
    if((bytes_received=recv(client_socket, server_response, sizeof(server_response), 0)) < 0) {
        warn("Receive failed");
    }
    else {
        server_response[bytes_received] = '\0';
        success("Received data");
        printf("Data: %s\n", server_response);
    }

    info("Closing sockets");
    close(client_socket);
    close(server_socket);

    return 0;
}
