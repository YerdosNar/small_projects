#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sodium.h>

#include "../include/netio.h"

#define PORT 8888
#define BACKLOG 8

int setup_listen_fd(uint16_t port) {
	int l_fd;
	struct sockaddr_in sa = {0};

	if((l_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	sa.sin_family 		= AF_INET;
	sa.sin_addr.s_addr 	= htonl(INADDR_ANY);
	sa.sin_port 		= htons(port);

	if (bind(l_fd, (struct sockaddr *)&sa , sizeof(sa)) < 0) {
		perror("bind");
		close(l_fd);
		exit(EXIT_FAILURE);
	}
	return l_fd;
}

int main(void) {
	if (sodium_init() < 0) {
		fprintf(stderr, "ERROR: main->sodium_init() failed.\n");
		exit(EXIT_FAILURE);
	}

	uint16_t port = PORT;
	int l_fd = setup_listen_fd(port);
	if (listen(l_fd, BACKLOG) < 0) {
		perror("listen");
		close(l_fd);
		exit(EXIT_FAILURE);
	}

	printf("Rendezvous: listening on port: %d\n", port);

	struct sockaddr_in ca = {0};
	socklen_t client_len = sizeof(ca);
	char client_ip[INET_ADDRSTRLEN];

	int client_fd = accept(l_fd, (struct sockaddr *)&ca, &client_len);
	if (client_fd < 0) {
		perror("accept");
		close(l_fd);
		exit(EXIT_FAILURE);
	}
	if (inet_ntop(AF_INET, &ca.sin_addr, client_ip, sizeof(client_ip)) == NULL) {
		perror("inet_ntop");
	} else {
		printf("Rendezvous: accepted connection from %s:%d (fd=%d)\n",
				client_ip, ntohs(ca.sin_port), client_fd);
	}

	uint8_t spk[crypto_kx_PUBLICKEYBYTES];
	uint8_t ssk[crypto_kx_SECRETKEYBYTES];
	uint8_t cpk[crypto_kx_PUBLICKEYBYTES];

	crypto_kx_keypair(spk, ssk);

	if (write_all(client_fd, ssk, sizeof(spk)) < 0) {
		perror("write_all(spk)");
		close(client_fd);
		close(l_fd);
		exit(EXIT_FAILURE);
	}

	if (read_all(client_fd, cpk, sizeof(cpk)) < 0) {
		perror("read_all(cpk)");
		close(client_fd);
		close(l_fd);
		exit(EXIT_FAILURE);
	}

	print_hex("Rendezvous pk (ours)", spk, sizeof(spk));
	print_hex("Peer pk (theis)     ", cpk, sizeof(cpk));

	sodium_memzero(ssk, sizeof(ssk));

	close(client_fd);
	close(l_fd);
	return 0;
}
