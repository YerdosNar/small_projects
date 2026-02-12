#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef LOGGER_H
#define LOGGER_H

#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define NOC "\033[0m"

void info(const char *msg, ...);
void warn(const char *msg, ...);
void success(const char *msg, ...);
void err(const char *msg, ...);

#endif
