#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <stdlib.h>

#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"

#define B_RED "\033[41m"
#define B_GRN "\033[42m"
#define B_YEL "\033[43m"
#define B_BLU "\033[44m"
#define NOC "\033[0m"

void info(const char *msg, ...);
void warn(const char *msg, ...);
void sccs(const char *msg, ...);
void err(const char *msg, ...);
void print_progress_bar(int percent);

// packet
#define T_EOF       1
#define T_DAT       2
#define T_ACK       3
#define PACKET_SIZE 1400
// packet size - sizeof(seq_num) - sizeof(type)
#define DATA_SIZE   1395
#include <stdint.h>
typedef struct __attribute__((packed)) {
    uint8_t type;
    uint32_t seq_num;
    uint8_t data[DATA_SIZE];
} packet_t;

#endif
