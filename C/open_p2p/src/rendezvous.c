#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sodium.h>
#include <string.h>
#include <stdbool.h>

#include "../include/netio.h"
#include "../include/crypto.h"

#define PORT 8888
#define BACKLOG 8

typedef struct {
	int fd;
	struct sockaddr_in addr;
	uint8_t pk[PUBKB];
	encrypted_channel ch;
} peer_conn_t;

typedef struct {
	char id[65];
	char pw[65];
	int fd;
	uint8_t pk[PUBKB];
	struct sockaddr_in addr;
	encrypted_channel ch;
	bool occupied;
} room_t;

static int accept_and_handshake(int l_fd, peer_conn_t *peer) {
	socklen_t addr_len = sizeof(peer->addr);
	char ip[INET_ADDRSTRLEN];

	peer->fd = accept(l_fd, (struct sockaddr*)&peer->addr, &addr_len);
	if (peer->fd < 0) {
		perror("accept");
		return -1;
	}

	inet_ntop(AF_INET, &peer->addr.sin_addr, ip, sizeof(ip));
	printf("Rendezvous: accepted connection from %s:%d (fd=%d)\n",
			ip, ntohs(peer->addr.sin_port), peer->fd);

	uint8_t spk[PUBKB], ssk[SECKB];
	if (crypto_keygen(spk, ssk)) return -1;

	if (write_all(peer->fd, spk, PUBKB) < 0) {
		perror("write_all(spk)"); return -1;
	}
	if (read_all(peer->fd, peer->pk, PUBKB) < 0) {
		perror("read-all(peer->pk)"); return -1;
	}

	uint8_t rx[SESKB], tx[SESKB];
	if (crypto_derivekeys(rx, tx, spk, ssk, peer->pk)) return -1;
	sodium_memzero(ssk, sizeof(ssk));

	if (crypto_channel_init(&peer->ch, peer->fd, tx, rx) < 0) return -1;
	sodium_memzero(rx, sizeof(rx));
	sodium_memzero(tx, sizeof(tx));

	return 0;
}

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

static int send_prompt(encrypted_channel *ch, int fd,
		const char *prompt,
		char *buf, size_t max_len) {
	if (crypto_channel_send(ch, fd,
				(const uint8_t*)prompt, strlen(prompt), 0) < 0) {
		return -1;
	}

	uint8_t tag;
	int n = crypto_channel_recv(ch, fd, (uint8_t*)buf, max_len - 1, &tag);
	if (n < 0) return -1;
	buf[n] = '\0';

	return n;
}

int main(void) {
	if (crypto_init()) exit(EXIT_FAILURE);

	uint16_t port = PORT;
	int l_fd = setup_listen_fd(port);
	if (listen(l_fd, BACKLOG) < 0) {
		perror("listen"); close(l_fd); exit(EXIT_FAILURE);
	}
	printf("Rendezvous: listening on port: %d\n", port);

	room_t room = {0};

	// ---- Wait for first peer ----
	peer_conn_t p1;
	if (accept_and_handshake(l_fd, &p1) < 0) {
		close(l_fd); exit(EXIT_FAILURE);
	}

	char answer[256];
	if (send_prompt(&p1.ch, p1.fd,
				"Are you [H]ost or [J]oin? [h/j]: ",
				answer, sizeof(answer)) < 0)
		goto fail_p1;

	if (answer[0] != 'h' && answer[0] != 'H') {
		// First peer must be host (no rooms exist yet)
		crypto_channel_send(&p1.ch, p1.fd,
				(const uint8_t *)"No rooms available. Goodbye.\n",
				strlen("No rooms available. Goodbye.\n"), 0);
		goto fail_p1;
	}

	// Host provides room ID and password
	if (send_prompt(&p1.ch, p1.fd,
				"Enter Room ID: ",
				room.id, sizeof(room.id)) < 0)
		goto fail_p1;

	if (send_prompt(&p1.ch, p1.fd,
				"Enter Room PW: ",
				room.pw, sizeof(room.pw)) < 0)
		goto fail_p1;

	room.fd = p1.fd;
	room.ch = p1.ch;
	room.addr = p1.addr;
	memcpy(room.pk, p1.pk, PUBKB);
	room.occupied = true;

	printf("Rendezvous: room created [id=%s]\n", room.id);

	// Notify host that we're waiting
	const char *wait_msg = "Room created. Waiting for peer to join...\n";
	crypto_channel_send(&room.ch, room.fd,
			(const uint8_t *)wait_msg, strlen(wait_msg), 0);

	// ---- Wait for second peer ----
	peer_conn_t p2;
	if (accept_and_handshake(l_fd, &p2) < 0) {
		goto fail_room;
	}

	if (send_prompt(&p2.ch, p2.fd,
				"Are you [H]ost or [J]oin? [h/j]: ",
				answer, sizeof(answer)) < 0)
		goto fail_p2;

	if (answer[0] != 'j' && answer[0] != 'J') {
		crypto_channel_send(&p2.ch, p2.fd,
				(const uint8_t *)"Only join is available. Goodbye.\n",
				strlen("Only join is available. Goodbye.\n"), 0);
		goto fail_p2;
	}

	// Joiner provides room ID and password
	char join_id[65], join_pw[65];
	if (send_prompt(&p2.ch, p2.fd,
				"Enter Room ID: ",
				join_id, sizeof(join_id)) < 0)
		goto fail_p2;

	if (send_prompt(&p2.ch, p2.fd,
				"Enter Room PW: ",
				join_pw, sizeof(join_pw)) < 0)
		goto fail_p2;

	// Check credentials
	if (strcmp(join_id, room.id) != 0 || strcmp(join_pw, room.pw) != 0) {
		printf("Rendezvous: join failed (wrong id/pw)\n");
		crypto_channel_send(&p2.ch, p2.fd,
				(const uint8_t *)"Invalid Room ID or Password. Goodbye.\n",
				strlen("Invalid Room ID or Password. Goodbye.\n"), 0);
		goto fail_p2;
	}

	printf("Rendezvous: peer joined room [id=%s] — match!\n", room.id);

	// Notify both peers
	const char *ok_msg = "Peer found! Connection established.\n";
	crypto_channel_send(&room.ch, room.fd,
			(const uint8_t *)ok_msg, strlen(ok_msg), 0);
	crypto_channel_send(&p2.ch, p2.fd,
			(const uint8_t *)ok_msg, strlen(ok_msg), 0);

	// Clean up room
	sodium_memzero(&room, sizeof(room));

	close(p2.fd);
	close(p1.fd);
	close(l_fd);
	return 0;

fail_p2:
	close(p2.fd);
fail_room:
	sodium_memzero(&room, sizeof(room));
fail_p1:
	close(p2.fd);
	close(l_fd);
	exit(EXIT_FAILURE);
}
