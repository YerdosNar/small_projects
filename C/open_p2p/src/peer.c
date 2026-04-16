#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

int connect_to_server(const char *ip, uint16_t port) {
	int fd;
	struct sockaddr_in sa = {0};

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	sa.sin_family = AF_INET;
	sa.sin_port   = htons(port);

	if (inet_pton(AF_INET, ip, &sa.sin_addr) <= 0) {
		perror("inet_pton");
		close(fd);
		exit(EXIT_FAILURE);
	}

	if (connect(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		perror("connect");
		close(fd);
		exit(EXIT_FAILURE);
	}

	return fd;
}

int main() {
	char *server_ip = SERVER_IP;
	uint16_t port = SERVER_PORT;
	int fd = connect_to_server(server_ip, port);
	printf("Peer: connected to %s:%d (fd=%d)\n",
			server_ip, port, fd);
	close(fd);
	return 0;
}
