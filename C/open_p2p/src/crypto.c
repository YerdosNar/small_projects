#include "../include/crypto.h"

int8_t cypto_keygen(Peer *p) {
	return crypto_kx_keypair(p->pk, p->sk);
}

uint8_t *crypto_encrypt(uint8_t *in_plain, uint8_t tx[TK_LEN]) {
	unsigned long long len = strlen(in_plain);
	uint8_t nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
	randombytes_buf(nonce, sizeof(nonce));

	unsigned long long cipher_len = len + crypto_aead_xchacha20poly1305_ietf_ABYTES;
	uint8_t uint8_t cipher[cipher_len];
	if (crypto_aead_xchacha20poly1305_ietf_encrypt(
				cipher, &cipher_len,
				in_plain, len,
				NULL, 0,
				NULL,
				nonce, client.tx
