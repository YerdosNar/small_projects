#include <stdio.h>

#include "../include/crypto.h"

int main() {
	int sockfd, connfd;
	struct sockaddr_in sa, ca;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "ERROR: main->socket()\n");
		return 1;
	}
	printf("Peer.\n");

	return 0;
}
