#include "../include/crypto.h"

#include <string.h>

int8_t crypto_keygen(uint8_t pubkey[PUBKB], uint8_t seckey[SECKB]) {
	if (sodium_init() < 0) {
		fprintf(stderr, "ERROR: crypto_keygen->sodium_init() failed.\n");
		return -1;
	}
	return (int8_t)crypto_kx_keypair(pubkey, seckey);
}

int8_t crypto_derivekeys(uint8_t rx[SESKB], uint8_t tx[SESKB], uint8_t my_pk[PUBKB], uint8_t my_sk[SECKB], uint8_t peer_pk[PUBKB]) {
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
