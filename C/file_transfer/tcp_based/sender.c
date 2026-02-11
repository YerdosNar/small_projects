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

void info(char *msg)    { printf(BLU "[i]" NOC " %s\n", msg);         }
void warn(char *msg)    { printf(YEL "[!]" NOC " %s\n", msg);         }
void success(char *msg) { printf(GRN "[✓]" NOC " %s\n", msg);         }
void err(char *msg)     { printf(RED "[x]" NOC " %s\n", msg);exit(1); }

int main(int argc, char **argv) {
    int network_socket;

    info("Creating socket");
    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err("Socket creation failed");
    }
    success("Socket created succesfully!");

    struct sockaddr_in server_address = {
        .sin_family      = AF_INET,     // IPv4
        .sin_port        = htons(8080), // Port number Host-to-Network
        .sin_addr.s_addr = INADDR_ANY   //
    };

    info("Connecting to remote socket");
    int con_status;
    if ((con_status=connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address))) == -1) {
        err("Connection to remote socket failed");
    }
    success("Connected to the server successfully");

    info("Sending data");
    char *message = "Hello from the sender!";
    ssize_t bytes_sent;
    if((bytes_sent=send(network_socket, message, strlen(message), 0)) < 0) {
        err("Failed to send data");
    }
    success("Data sent successfully");

    info("Closing socket");
    close(network_socket);
    success("Goodbye!");

    return 0;
}
