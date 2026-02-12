#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef LOGGER_H
#define LOGGER_H

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
void success(const char *msg, ...);
void err(const char *msg, ...);

#endif
