#include "../include/crypto.h"

int8_t cypto_keygen(Peer *p) {
	return crypto_kx_keypair(p->pk, p->sk);
}


