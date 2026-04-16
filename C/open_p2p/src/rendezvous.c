#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sodium.h>
#include <string.h>

#include "../include/netio.h"
#include "../include/crypto.h"

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
	if (crypto_init()) exit(EXIT_FAILURE);

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

	uint8_t spk[PUBKB];
	uint8_t ssk[SECKB];
	uint8_t cpk[PUBKB];
	if (crypto_keygen(spk, ssk)) {
		fprintf(stderr, "ERROR: main()->crypto_keygen() failed.\n");
		close(l_fd);
		exit(EXIT_FAILURE);
	}

	if (write_all(client_fd, spk, sizeof(spk)) < 0) {
		perror("write_all(spk)");
		goto fail;
	}

	if (read_all(client_fd, cpk, sizeof(cpk)) < 0) {
		perror("read_all(cpk)");
		goto fail;
	}

	print_hex("Rendezvous pk (ours)", spk, sizeof(spk));
	print_hex("Peer pk (theirs)    ", cpk, sizeof(cpk));

	uint8_t rx[SESKB];
	uint8_t tx[SESKB];

	if (crypto_derivekeys(rx, tx, spk, ssk, cpk)) {
		fprintf(stderr, "ERROR: crypto_kx_server_session_keys failed (bad peer pk)\n");
		goto fail;
	}
	sodium_memzero(ssk, sizeof(ssk));

	encrypted_channel ch;
	if (crypto_channel_init(&ch, client_fd, tx, rx) < 0) goto fail;

	sodium_memzero(rx, sizeof(rx));
	sodium_memzero(tx, sizeof(tx));

	const char *msg = "hello from rendezvous";
	if (crypto_channel_send(&ch, client_fd,
				(const uint8_t *)msg, strlen(msg), 0) < 0) goto fail;
	printf("Rendezvous: send '%s'\n", msg);

	uint8_t buf[1024];
	uint8_t tag;
	int n = crypto_channel_recv(&ch, client_fd, buf, sizeof(buf) - 1, &tag);
	if (n < 0) goto fail;
	buf[n] = '\0';
	printf("Rendezvous: received '%s' (tag=%u)\n", buf, tag);

	close(client_fd);
	close(l_fd);
	return 0;

fail:
	close(client_fd);
	close(l_fd);
	exit(EXIT_FAILURE);
}
