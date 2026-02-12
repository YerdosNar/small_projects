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

void print_progress_bar(int percentage) {
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);

    int cols = w.ws_col;
    int bar_width = cols - 8; // cols - [] - 100%
    int fill_bar = percentage * bar_width / 100;

    printf("\r[");
    for(int i = 0; i < bar_width; i++) {
        if(i < fill_bar) {
            printf(B_GRN " ");
        }
        else {
            printf(NOC " ");
        }
    }
    printf("] %3d%%", percentage);
    fflush(stdout);
}

