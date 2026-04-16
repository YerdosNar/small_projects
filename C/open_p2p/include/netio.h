#ifndef NETIO_H
#define NETIO_H

#include <stdint.h>
#include <stddef.h>

int write_all(int fd, const void *buf, size_t n);
int read_all(int fd, void *buf, size_t n);

int write_frame(int fd, const void *buf, uint32_t n);
int read_frame(int fd, void *buf, uint32_t max_n, uint32_t *out_n);

void print_hex(const char *label, const uint8_t *buf, size_t n);

#endif
