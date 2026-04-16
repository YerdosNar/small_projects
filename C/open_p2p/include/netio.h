#ifndef NETIO_H
#define NETIO_H

#include <stdint.h>
#include <stddef.h>

int write_all(int fd, const void *buf, size_t n);

int read_all(int fd, void *buf, size_t n);

void print_hex(const char *label, const uint8_t *buf, size_t n);

#endif
