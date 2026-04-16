#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <sodium.h>

#include "../include/netio.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

static int resolve_domain_name(const char *domain, char *out_ip, size_t out_len)
{
	struct addrinfo hints = {0};
	struct addrinfo *res = NULL;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	int rc = getaddrinfo(domain, NULL, &hints, &res);
	if (rc != 0) {
		fprintf(stderr, "ERROR: resolve_domain_name: getaddrinfo(%s): %s\n",
				domain, gai_strerror(rc));
		return -1;
	}

	struct sockaddr_in *sa = (struct sockaddr_in *)res->ai_addr;
	if (inet_ntop(AF_INET, &sa->sin_addr, out_ip, out_len) == NULL) {
		perror("inet_ntop");
		freeaddrinfo(res);
		return -1;
	}

	freeaddrinfo(res);
	return 0;
}

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

int main(int argc, char **argv) {
	if (sodium_init() < 0) {
		fprintf(stderr, "ERROR: main->sodium_init() failed.\n");
		exit(EXIT_FAILURE);
	}

	char server_ip[INET_ADDRSTRLEN];

	if (argc >= 2) {
		if (resolve_domain_name(argv[1], server_ip, sizeof(server_ip)) < 0) {
			exit(EXIT_FAILURE);
		}
	}
	else {
		strncpy(server_ip, SERVER_IP, sizeof(server_ip));
		server_ip[sizeof(server_ip) - 1] = '\0';
	}

	uint16_t port = SERVER_PORT;
	int fd = connect_to_server(server_ip, port);
	printf("Peer: connected to %s:%d (fd=%d)\n",
			server_ip, port, fd);

	uint8_t cpk[crypto_kx_PUBLICKEYBYTES];
	uint8_t csk[crypto_kx_SECRETKEYBYTES];
	uint8_t spk[crypto_kx_PUBLICKEYBYTES];

	crypto_kx_keypair(cpk, csk);

	if (read_all(fd, spk, sizeof(spk)) < 0) {
		perror("read_all(server_pk)");
		close(fd);
		exit(EXIT_FAILURE);
	}

	if (write_all(fd, cpk, sizeof(cpk)) < 0) {
		perror("write_all(server_pk)");
		close(fd);
		exit(EXIT_FAILURE);
	}

	print_hex("Peer pk (ours)	 : ", cpk, sizeof(cpk));
	print_hex("Rendezvous pk (theirs): ", spk, sizeof(spk));

	uint8_t rx[crypto_kx_SESSIONKEYBYTES];
	uint8_t tx[crypto_kx_SESSIONKEYBYTES];

	int32_t cmp = memcmp(cpk, spk, crypto_kx_PUBLICKEYBYTES);
	if (cmp > 0) {
		if (crypto_kx_server_session_keys(rx, tx, cpk, csk, spk) != 0) {
			fprintf(stderr, "ERROR: crypto_kx_server_session_keys failed (bad peer pk)\n");
			close(fd);
			exit(EXIT_FAILURE);
		}
	} else if (cmp < 0) {
		if (crypto_kx_client_session_keys(rx, tx, cpk, csk, spk) != 0) {
			fprintf(stderr, "ERROR: crypto_kx_client_session_keys failed (bad peer pk)\n");
			close(fd);
			exit(EXIT_FAILURE);
		}
	}
	sodium_memzero(csk, sizeof(csk));

	print_hex("rx (server receives on) : ", rx, sizeof(rx));
	print_hex("tx (server transmits on): ", tx, sizeof(tx));

	close(fd);
	return 0;
}
