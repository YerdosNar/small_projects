#include <stdio.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define RED   "\033[31m"
#define B_RED "\033[41m"
#define GRN   "\033[32m"
#define B_GRN "\033[42m"
#define YEL   "\033[33m"
#define B_YEL "\033[43m"
#define BLU   "\033[34m"
#define B_BLU "\033[44m"
#define BLD   "\033[1m"
#define NOC   "\033[0m"

const char *digits[11][5] = {
    // 0
    {
        "  ###  ",
        " #   # ",
        " #   # ",
        " #   # ",
        "  ###  "
    },
    // 1
    {
        "   #   ",
        "  ##   ",
        "   #   ",
        "   #   ",
        "  ###  "
    },
    // 2
    {
        "  ###  ",
        " #   # ",
        "    #  ",
        "   #   ",
        " ##### "
    },
    // 3
    {
        "  ###  ",
        " #   # ",
        "   ##  ",
        " #   # ",
        "  ###  "
    },
    // 4
    {
        "   #   ",
        "  ##   ",
        " # #   ",
        " ##### ",
        "   #   "
    },
    // 5
    {
        " ##### ",
        " #     ",
        " ##### ",
        "     # ",
        " ##### "
    },
    // 6
    {
        "  ###  ",
        " #     ",
        " ####  ",
        " #   # ",
        "  ###  "
    },
    // 7
    {
        " ##### ",
        "     # ",
        "    #  ",
        "   #   ",
        "   #   "
    },
    // 8
    {
        "  ###  ",
        " #   # ",
        "  ###  ",
        " #   # ",
        "  ###  "
    },
    // 9
    {
        "  ###  ",
        " #   # ",
        "  #### ",
        "     # ",
        "  ###  "
    },
    // : (Colon - Index 10)
    {
        "       ",
        "   ##  ",
        "       ",
        "   ##  ",
        "       "
    }
};

const char time_up[5][47] = {
    "##### ##### #   # #####       #   # ####    #",
    "  #     #   ## ## #           #   # #   #   #",
    "  #     #   # # # #####       #   # ####    #",
    "  #     #   #   # #           #   # #        ",
    "  #   ##### #   # #####        ###  #       #"
};


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

void print_big_timer(int hours, int minutes, int seconds, int height, int width) {
    int total = hours * 3600 + minutes * 60 + seconds;
    int y_pos = (height - 5) / 2;
    int x_pos = (width - 5 * 8) / 2;
    system("clear");

    while(total >= 0) {
        printf("\033[%d;%dH", y_pos, x_pos);
        total--;
        printf("Hello\n");
    }

    system("clear");
    int red = 0;
    while(1) {
        printf("\033[%d;%dH", y_pos, x_pos);
        if (red) {
            for(int i = 0; i < 5; i++) {
                printf("\033[%d;%dH", y_pos+i, x_pos);
                printf(BLD B_RED BLU "%s\n", time_up[i]);
                red = 0;
            }
        }
        else {
            for(int i = 0; i < 5; i++) {
                printf("\033[%d;%dH", y_pos+i, x_pos);
                printf(BLD B_BLU RED "%s\n", time_up[i]);
                red = 1;
            }
        }
        fflush(stdout);
        sleep(1);
    }

}

void print_digit(int num) {
    for (int i = 0; i < 5; i++) {
        printf("%s\n", digits[num][i]);
    }
}
void usage(char *exe) {
    printf("Usage: %s [options]\n", exe);
    printf("Options: \n");
    printf("  -h,--hours <number>         to set hours\n");
    printf("  -m,--minutes <number>       to set minutes\n");
    printf("  -s,--seconds <number>       to set seconds\n");
    printf("Example:\n");
    printf("  %s -h 1 -m 10 -s 20\n", exe);
    exit(0);
}

void signal_handler(int sig) {
    printf("\n=> Bye");
    for (int i = 0; i < 3; i++) {
        sleep(1);
        printf(".");
        fflush(stdout);
    }
    printf("\n");
    exit(sig);
}

int main(int argc, char **argv) {
    signal(SIGINT, signal_handler);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = w.ws_col;
    int height = w.ws_row;
    // int x_pixel = w.ws_xpixel;
    // int y_pixel = w.ws_ypixel;

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
        print_big_timer(hours, minutes, seconds, height, width);
    }
    else {
        print_small_timer(hours, minutes, seconds);
    }

    return 0;
}
