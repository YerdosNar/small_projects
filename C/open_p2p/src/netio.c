#include "../include/netio.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int write_all(int fd, const void *buf, size_t n) {
	const uint8_t *p = buf;
	while(n > 0) {
		ssize_t w = write(fd, p, n);
		if (w < 0) {
			if (errno == EINTR) continue;
			return -1;
		}
		p += w;
		n -= (size_t)w;
	}
	return 0;
}

int read_all(int fd, void *buf, size_t n) {
	uint8_t *p = buf;
	while (n > 0) {
		ssize_t r = read(fd, p, n);
		if (r < 0) {
			if (errno == EINTR) continue;
			return -1;
		}
		p += r;
		n -= (size_t)r;
	}
	return 0; 
} 

void print_hex(const char *label, const uint8_t *buf, size_t n) {
	printf("%s: ", label);
	for (size_t i = 0; i < n; i++) printf("%02x", buf[i]);
	printf("\n");
}

