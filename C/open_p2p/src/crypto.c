#include "../include/crypto.h"
#include "../include/netio.h"

#include <string.h>

int crypto_channel_init(encrypted_channel *ch, int fd, 
		const uint8_t tx_key[SESKB],
		const uint8_t rx_key[SESKB])
{
	uint8_t tx_hdr[HDR];
	uint8_t rx_hdr[HDR];

	if (crypto_secretstream_xchacha20poly1305_init_push(
				&ch->tx, tx_hdr, tx_key) != 0) {
		fprintf(stderr, "ERROR: init_push() failed.\n");
		return -1;
	}

	if (write_all(fd, tx_hdr, HDR) < 0) {
		perror("write_all(tx_hdr)");
		return -1;
	}
	if (read_all(fd, rx_hdr, HDR) < 0) {
		perror("read_all(rx_hdr)");
		return -1;
	}

	if (crypto_secretstream_xchacha20poly1305_init_pull(
				&ch->rx, rx_hdr, rx_key) != 0) {
		fprintf(stderr, "ERROR: init_pull() failed.\n");
		return -1;
	}

	return 0;
}

// Encrypt and send msg
int crypto_channel_send(encrypted_channel *ch, int fd,
		const uint8_t *msg, size_t msg_len,
		uint8_t tag)
{
	uint8_t ctext[msg_len + ABYTES];
	unsigned long long clen;

	if (crypto_secretstream_xchacha20poly1305_push(
				&ch->tx,
				ctext, &clen,
				msg, msg_len,
				NULL, 0,
				tag) != 0) {
		fprintf(stderr, "ERROR: secretstream_push() failed.\n");
		return -1;
	}

	if (write_frame(fd, ctext, (uint32_t)clen) < 0) {
		perror("write_frame");
		return -1;
	}

	return 0;
}

// Receive and decrypt msg
// plain text written into buf (up to max_len)
// Returns actual plaintext length, or -1 on error.
// *out_tag receives the 'tag'
int crypto_channel_recv(encrypted_channel *ch, int fd,
		uint8_t *buf, size_t max_len,
		uint8_t *out_tag)
{
	uint8_t ctext[max_len + ABYTES];
	uint32_t clen;

	if (read_frame(fd, ctext, sizeof(ctext), &clen) < 0) {
		perror("read_frame");
		return -1;
	}

	unsigned long long plen;
	if (crypto_secretstream_xchacha20poly1305_pull(
				&ch->rx,
				buf, &plen,
				out_tag,
				ctext, clen,
				NULL, 0) != 0) {
		fprintf(stderr, "ERROR: secretstream_pull() failed (tampered or wrong key)\n");
		return -1;
	}

	return (int)plen;
}

int crypto_init() {
	if (sodium_init() < 0) {
		fprintf(stderr, "ERROR: crypto_keygen->sodium_init() failed.\n");
		return -1;
	}
	return 0;
}

int crypto_keygen(uint8_t pubkey[PUBKB], uint8_t seckey[SECKB]) {
	return (int8_t)crypto_kx_keypair(pubkey, seckey);
}

int crypto_derivekeys(uint8_t rx[SESKB], uint8_t tx[SESKB], uint8_t my_pk[PUBKB], uint8_t my_sk[SECKB], uint8_t peer_pk[PUBKB]) {
	int cmp = memcmp(my_pk, peer_pk, PUBKB);
	if (cmp == 0) {
		fprintf(stderr, "ERROR: public keys equal(loopback)?\n");
		return -1;
	}

	int8_t ret;
	if (cmp < 0) {
		ret = crypto_kx_client_session_keys(
				rx, tx, my_pk, my_sk, peer_pk);
	} else {
		ret = crypto_kx_server_session_keys(
				rx, tx, my_pk, my_sk, peer_pk);
	}

	if (ret != 0) {
		fprintf(stderr, "ERROR: crypto_derivekeys failed\n");
		return -1;
	}

	return 0;
}
