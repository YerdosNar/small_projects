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

#include "../include/netio.h"
#include "../include/crypto.h"

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
	if (crypto_init()) exit(EXIT_FAILURE);

	char server_ip[INET_ADDRSTRLEN];
	if (argc >= 2) {
		if (resolve_domain_name(argv[1], server_ip, sizeof(server_ip)) < 0) {
			exit(EXIT_FAILURE);
		}
	}
	else {
		snprintf(server_ip, sizeof(server_ip), "%s", SERVER_IP);
	}

	uint16_t port = SERVER_PORT;
	int fd = connect_to_server(server_ip, port);
	printf("Peer: connected to %s:%d (fd=%d)\n",
			server_ip, port, fd);

	uint8_t cpk[PUBKB];
	uint8_t csk[SECKB];
	uint8_t spk[PUBKB];

	// init sodium and generate keys
	if (crypto_keygen(cpk, csk)) {
		fprintf(stderr, "ERROR: main->crypto_keygen() failed.\n");
		goto fail;
	}

	if (read_all(fd, spk, sizeof(spk)) < 0) {
		perror("read_all(server_pk)");
		goto fail;
	}

	if (write_all(fd, cpk, sizeof(cpk)) < 0) {
		perror("write_all(server_pk)");
		goto fail;
	}

	print_hex("Peer pk (ours)	 ", cpk, sizeof(cpk));
	print_hex("Rendezvous pk (theirs)", spk, sizeof(spk));

	uint8_t rx[SESKB];
	uint8_t tx[SESKB];

	if (crypto_derivekeys(rx, tx, cpk, csk, spk)) {
		fprintf(stderr, "ERROR: main()->crypto_derivekeys() failed.\n");
		goto fail;
	}
	sodium_memzero(csk, sizeof(csk));

	encrypted_channel ch;
	if (crypto_channel_init(&ch, fd, tx, rx) < 0) goto fail;

	sodium_memzero(rx, sizeof(rx));
	sodium_memzero(tx, sizeof(tx));

	uint8_t buf[1024];
	uint8_t tag;
	int n = crypto_channel_recv(&ch, fd, buf, sizeof(buf) - 1, &tag);
	if (n < 0) goto fail;
	buf[n] = '\0';
	printf("Peer: received '%s' (tag=%u)\n", buf, tag);

	const char *reply = "hello from peer";
	if (crypto_channel_send(&ch, fd,
				(const uint8_t *)reply, strlen(reply), 0) < 0) goto fail;
	printf("Peer: sent '%s'\n", reply);

	close(fd);
	return 0;

fail:
	close(fd);
	exit(EXIT_FAILURE);
}
