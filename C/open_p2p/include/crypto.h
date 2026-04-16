#ifndef CRYPTO_H
#define CRYPTO_H

#include <sodium.h>
#include <stdint.h>

#define PUBKB crypto_kx_PUBLICKEYBYTES
#define SECKB crypto_kx_SECRETKEYBYTES
#define SESKB crypto_kx_SESSIONKEYBYTES

int8_t crypto_keygen(uint8_t pubkey[PUBKB], uint8_t seckey[SECKB]);
int8_t crypto_derivekeys(uint8_t rx[SESKB], uint8_t tx[SESKB], uint8_t my_pk[PUBKB], uint8_t my_sk[SECKB], uint8_t peer_pk[PUBKB]);

#endif
