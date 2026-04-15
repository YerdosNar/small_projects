#ifndef CRYPTO_H
#define CRYPTO_H

#define PK_LEN crypto_kx_PUBLICKEYBYTES
#define SK_KEN crypto_kx_SECRETKEYBYTES
#define RK_LEN crypto_aead_xchacha20poly1305_ietf_KEYBYTES
#define TK_LEN crypto_aead_xchacha20poly1305_ietf_KEYBYTES

int8_t crypto_keygen(Peer *p);
int8_t crypto_encrypt(uint8_t *in_plain, uint8_t *out_crypt, uint8_t tx[TK_LEN]);

#endif
