#ifndef PEER_H
#define PEER_H

#include "crypt.h"

typedef struct {
	uint8_t 	pk[PK_LEN];
	uint8_t 	sk[SK_LEN];
	uint8_t 	ip[MAX_IP_LEN];
	uint16_t 	port;
} peer_t;

#endif
