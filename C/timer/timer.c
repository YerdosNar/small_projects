#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define RED   "\033[31m"
#define GRN   "\033[32m"
#define YEL   "\033[33m"
#define BLU   "\033[34m"

#define B_RED "\033[41m"
#define B_GRN "\033[42m"
#define B_YEL "\033[43m"
#define B_BLU "\033[44m"

#define BLD   "\033[1m"
#define NOC   "\033[0m"

#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

int width, height;

const char *digits[11][7] = {
    // 0
    {
        "  ######  ",
        " ##     ##",
        " ##     ##",
        " ##     ##",
        " ##     ##",
        " ##     ##",
        "  ######  "
    },
    // 1
    {
        "    ##   ",
        "  ####   ",
        "    ##   ",
        "    ##   ",
        "    ##   ",
        "    ##   ",
        "  ###### "
    },
    // 2
    {
        "  ######  ",
        " ##     ##",
        "        ##",
        "   #####  ",
        " ##       ",
        " ##       ",
        " ######## "
    },
    // 3
    {
        "  ######  ",
        " ##     ##",
        "        ##",
        "   #####  ",
        "        ##",
        " ##     ##",
        "  #######  "
    },
    // 4
    {
        " ##    ## ",
        " ##    ## ",
        " ##    ## ",
        " ######## ",
        "       ## ",
        "       ## ",
        "       ## "
    },
    // 5
    {
        " ######## ",
        " ##       ",
        " ##       ",
        " #######  ",
        "        ##",
        " ##     ##",
        "  ######   "
    },
    // 6
    {
        "  #####   ",
        " ##       ",
        " ##       ",
        " #######  ",
        " ##     ##",
        " ##     ##",
        "  #####  "
    },
    // 7
    {
        " ######## ",
        "       ## ",
        "      ##  ",
        "     ##   ",
        "    ##    ",
        "   ##     ",
        "   ##     "
    },
    // 8
    {
        "  ######  ",
        " ##     ##",
        " ##     ##",
        "  ######  ",
        " ##     ##",
        " ##     ##",
        "  ####### "
    },
    // 9
    {
        "  ######  ",
        " ##     ##",
        " ##     ##",
        "  ####### ",
        "        ##",
        "        ##",
        "  ######   "
    },
    // :
    {
        "          ",
        "    ##    ",
        "    ##    ",
        "          ",
        "    ##    ",
        "    ##    ",
        "          "
    }
};

const char time_up[6][53] = {
    "####### ##### #     # #######       #    # ####    #",
    "   #      #   ##   ## #             #    # #   #   #",
    "   #      #   # # # # ####          #    # #   #   #",
    "   #      #   #  #  # ####          #    # ####    #",
    "   #      #   #     # #             #    # #        ",
    "   #    ##### #     # #######        ####  #       #"
};

void print_digit(int num, int x_pos, int y_pos) {
    system("clear");
    printf(HIDE_CURSOR);
    printf("\033[%d;%dH", y_pos, x_pos);
    for(int i = 0; i < 7; i++) {
        printf("\033[%d;%dH", y_pos + i, x_pos);
        printf("%s", digits[num][i]);
    }
}

void print_big_timer(int hours, int minutes, int seconds) {
    int total = hours * 3600 + minutes * 60 + seconds;
    int x_pos = (width - 8 * 10) / 2;
    int y_pos = (height - 7) / 2;
    while (total >= 0) {
        int first_h = hours / 10;
        int second_h = hours % 10;
        int first_m = minutes / 10;
        int second_m = minutes % 10;
        int first_s = seconds / 10;
        int second_s = seconds % 10;
        printf("%d%d:%d%d:%d%d\n", first_h, second_h, first_m, second_m, first_s, second_s);
        printf(RED);
        print_digit(first_h, x_pos, y_pos);
        print_digit(second_h, x_pos + 11, y_pos);
        printf(NOC);
        print_digit(10, x_pos + 22, y_pos);
        printf(GRN);
        print_digit(first_m, x_pos + 33, y_pos);
        print_digit(second_m, x_pos + 44, y_pos);
        printf(NOC);
        print_digit(10, x_pos + 55, y_pos);
        printf(BLU);
        print_digit(first_s, x_pos + 66, y_pos);
        print_digit(second_s, x_pos + 77, y_pos);
        printf(NOC);
        fflush(stdout);

        total--;
        seconds--;
        if (seconds < 0) {
            seconds = 59;
            minutes--;
            if (minutes < 0) {
                minutes = 59;
                hours--;
            }
        }
        sleep(1);
    }
    printf("\n");
    printf(SHOW_CURSOR);
}

void print_small_timer(int hours, int minutes, int seconds) {
    int total = hours * 3600 + minutes * 60 + seconds;
    printf("Timer: \n");
    printf(RED "HH" NOC ":"
           GRN "MM" NOC ":"
           BLU "SS" NOC "\n");
    while (total >= 0) {
        printf("\r");
        printf(RED "%.2d" NOC ":"
               GRN "%.2d" NOC ":"
               BLU "%.2d" NOC , hours, minutes, seconds);
        fflush(stdout);
        total--;
        seconds--;
        if (seconds < 0) {
            seconds = 59;
            minutes--;
            if (minutes < 0) {
                minutes = 59;
                hours--;
            }
        }
        sleep(1);
    }

    printf("\n");
    while(1) {
        printf(BLD GRN "\rTIME UP!" NOC);
        fflush(stdout);
        sleep(1);
        printf(BLD RED "\rTIME UP!" NOC);
        fflush(stdout);
        sleep(1);
    }
}

void usage(char *exe) {
    printf("Usage: %s [options]\n", exe);
    printf("Options: \n");
    printf("  -h, --hours   <number>       to set hours\n");
    printf("  -m, --minutes <number>       to set minutes\n");
    printf("  -s, --seconds <number>       to set seconds\n");
    printf("  -n, --no-big                 use small text mode\n");
    printf("Example:\n");
    printf("  %s -h 1 -m 10 -s 20\n", exe);
    exit(0);
}

void signal_handler(int sig) {
    printf(SHOW_CURSOR);
    printf("\n");
    system("stty echo");
    exit(sig);
}

int main(int argc, char **argv) {
    signal(SIGINT, signal_handler);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    width = w.ws_col;
    height = w.ws_row;

    int hours = 0,
        minutes = 0,
        seconds = 0;
    int set_big_timer = 1;

    if (argc < 2) {
        printf("Enter hours: ");
        scanf("%d", &hours);
        printf("Enter minutes: ");
        scanf("%d", &minutes);
        printf("Enter seconds: ");
        scanf("%d", &seconds);
    }
    else {
        for (int i = 1; i < argc; i++) {
            if (!strncmp("-h", argv[i], 2) || !strncmp("--hours", argv[i], 7)) {
                if (i + 1 >= argc) {
                    fprintf(stderr, "ERROR: %s flag requires followed number.", argv[i]);
                    printf("Setting hours to 0\n");
                    hours = 0;
                }
                else {
                    hours = atoi(argv[i+1]);
                }
            }
            else if (!strncmp("-m", argv[i], 2) || !strncmp("--minutes", argv[i], 9)) {
                if (i + 1 >= argc) {
                    fprintf(stderr, "ERROR: %s flag requires followed number.", argv[i]);
                    printf("Setting minutes to 0\n");
                    minutes = 0;
                }
                else {
                    minutes = atoi(argv[i+1]);
                    if (minutes > 60) {
                        printf(YEL "WARNING! 0 <= Minutes < 60\n" NOC);
                        printf("Setting minutes to 59\n");
                        minutes = 59;
                    }
                }
            }
            else if (!strncmp("-s", argv[i], 2) || !strncmp("--seconds", argv[i], 9)) {
                if (i + 1 >= argc) {
                    fprintf(stderr, "ERROR: %s flag requires followed number.", argv[i]);
                    printf("Setting seconds to 0\n");
                    seconds = 0;
                }
                else {
                    seconds = atoi(argv[i+1]);
                    if (seconds > 60) {
                        printf(YEL "WARNING! 0 <= Seconds < 60\n" NOC);
                        printf("Setting seconds to 59\n");
                        seconds = 59;
                    }
                }
            }
            else if (!strncmp("-n", argv[i], 2) || !strncmp("--no-big", argv[i], 8)) {
                if (argc < 3) {
                    printf("Enter hours: ");
                    scanf("%d", &hours);
                    printf("Enter minutes: ");
                    scanf("%d", &minutes);
                    printf("Enter seconds: ");
                    scanf("%d", &seconds);
                }
                set_big_timer = 0;
            }
            else if (!strncmp("--help", argv[i], 6)) {
                usage(argv[0]);
            }
        }
    }

    if (hours == 0 && minutes == 0 && seconds == 0) {
        fprintf(stderr, "ERROR: Everything is zero...\n");
        return 0;
    }
    if (set_big_timer) {
        print_big_timer(hours, minutes, seconds);
    }
    else {
        print_small_timer(hours, minutes, seconds);
    }

    return 0;
}
