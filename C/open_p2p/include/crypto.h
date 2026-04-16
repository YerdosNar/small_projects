#ifndef CRYPTO_H
#define CRYPTO_H

#include <sodium.h>
#include <stdint.h>

#define PUBKB 	crypto_kx_PUBLICKEYBYTES
#define SECKB 	crypto_kx_SECRETKEYBYTES
#define SESKB 	crypto_kx_SESSIONKEYBYTES
#define HDR 	crypto_secretstream_xchacha20poly1305_HEADERBYTES
#define ABYTES 	crypto_secretstream_xchacha20poly1305_ABYTES

typedef struct {
	crypto_secretstream_xchacha20poly1305_state tx;
	crypto_secretstream_xchacha20poly1305_state rx;
} encrypted_channel;

// sodium_init() wrapper
int crypto_init();

// Init both stream directions
// fd -> exchange header over it
// tx -> transmit key
// rx -> receive key
int crypto_channel_init(encrypted_channel *ch, int fd, 
		const uint8_t tx_key[SESKB],
		const uint8_t rx_key[SESKB]);

// Encrypt and send msg
int crypto_channel_send(encrypted_channel *ch, int fd,
		const uint8_t *msg, size_t msg_len,
		uint8_t tag);

// Receive and decrypt msg
// plain text written into buf (up to max_len)
// Returns actual plaintext length, or -1 on error.
// *out_tag receives the 'tag'
int crypto_channel_recv(encrypted_channel *ch, int fd,
		uint8_t *buf, size_t max_len,
		uint8_t *out_tag);

// Just keypair wrapper
int crypto_keygen(uint8_t pubkey[PUBKB], uint8_t seckey[SECKB]);

// Derive key after comparing them
int crypto_derivekeys(uint8_t rx[SESKB], uint8_t tx[SESKB], uint8_t my_pk[PUBKB], uint8_t my_sk[SECKB], uint8_t peer_pk[PUBKB]);

#endif
