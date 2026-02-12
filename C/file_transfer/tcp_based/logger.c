#include "logger.h"

void info(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    printf(BLU "[i]" NOC " ");
    vprintf(msg, args);
    printf("\n");
    va_end(args);
}
void warn(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    printf(YEL "[!]" NOC " ");
    vprintf(msg, args);
    printf("\n");
    va_end(args);
}
void success(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    printf(GRN "[✓]" NOC " ");
    vprintf(msg, args);
    printf("\n");
    va_end(args);
}
void err(const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    printf(RED "[x]" NOC " ");
    vprintf(msg, args);
    printf("\n");
    va_end(args);

    exit(1);
}

