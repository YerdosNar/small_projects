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
#define MAG   "\033[35m"
#define CYN   "\033[36m"

#define BLD   "\033[1m"
#define NOC   "\033[0m"

#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
#define CLEAR_SCREEN "\033[2J\033[H"

int width, height;

// YOUR 7-ROW DIGITS
const char *digits[11][7] = {
    // 0
    {"  ######  ", " ##    ## ", " ##    ## ", " ##    ## ", " ##    ## ", " ##    ## ", "  ######  "},
    // 1
    {"    ##    ", "   ####   ", "    ##    ", "    ##    ", "    ##    ", "    ##    ", "  ######  "},
    // 2
    {"  ######  ", " ##    ## ", "       ## ", "   #####  ", " ##       ", " ##       ", " ######## "},
    // 3
    {"  ######  ", " ##    ## ", "       ## ", "   #####  ", "       ## ", " ##    ## ", "  ####### "},
    // 4
    {" ##    ## ", " ##    ## ", " ##    ## ", " ######## ", "       ## ", "       ## ", "       ## "},
    // 5
    {" ######## ", " ##       ", " ##       ", " #######  ", "       ## ", " ##    ## ", "  ######  "},
    // 6
    {"  #####   ", " ##       ", " ##       ", " #######  ", " ##    ## ", " ##    ## ", "  #####   "},
    // 7
    {" ######## ", "       ## ", "      ##  ", "     ##   ", "    ##    ", "    ##    ", "    ##    "},
    // 8
    {"  ######  ", " ##    ## ", " ##    ## ", "  ######  ", " ##    ## ", " ##    ## ", "  ####### "},
    // 9
    {"  ######  ", " ##    ## ", " ##    ## ", "  ####### ", "       ## ", "       ## ", "  ######  "},
    // : (Colon - Index 10)
    {"          ", "    ##    ", "    ##    ", "          ", "    ##    ", "    ##    ", "          "}
};

// YOUR 6-ROW TIME UP TEXT
const char time_up[6][53] = {
    "####### ##### #     # #######        #    # ####    #",
    "   #      #   ##   ## #              #    # #   #   #",
    "   #      #   # # # # ####           #    # #   #   #",
    "   #      #   #  #  # ####           #    # ####    #",
    "   #      #   #     # #              #    # #        ",
    "   #    ##### #     # #######        ####   #       #"
};

void cleanup_and_exit(int sig) {
    printf(SHOW_CURSOR); // Restore cursor
    printf(NOC);         // Reset colors
    printf("\n");
    system("stty echo"); // Ensure input echo is back
    exit(sig);
}

void print_time_up_animation() {
    printf(CLEAR_SCREEN);

    // Calculate center for the message
    // The message is 53 chars wide and 6 rows high
    int start_y = (height - 6) / 2;
    int start_x = (width - 53) / 2;
    if (start_x < 0) start_x = 0;
    if (start_y < 0) start_y = 0;

    int flash = 0;

    while(1) {
        // Toggle color between RED and YELLOW
        char *color = flash ? BLD RED : BLD YEL;

        // Print the block
        for(int r = 0; r < 6; r++) {
            printf("\033[%d;%dH", start_y + r, start_x);
            printf("%s%s" NOC, color, time_up[r]);
        }

        fflush(stdout);
        flash = !flash;
        sleep(1);
    }
}

void print_big_timer(int hours, int minutes, int seconds) {
    int total = hours * 3600 + minutes * 60 + seconds;

    // Approximate width of the clock (8 blocks of roughly 10 chars)
    int clock_width = 84;
    int start_x = (width - clock_width) / 2;
    int start_y = (height - 7) / 2; // 7 is height of digits

    if (start_x < 0) start_x = 0;
    if (start_y < 0) start_y = 0;

    printf(CLEAR_SCREEN); // Clear once at start

    while (total >= 0) {
        int digits_indices[8] = {
            hours / 10, hours % 10,
            10, // colon
            minutes / 10, minutes % 10,
            10, // colon
            seconds / 10, seconds % 10
        };

        // LOOP 1: Iterate through the 7 ROWS of the block font
        for (int row = 0; row < 7; row++) {

            // Move cursor to the start of this specific row
            printf("\033[%d;%dH", start_y + row, start_x);

            // LOOP 2: Iterate through the 8 DIGITS (HH:MM:SS)
            for (int i = 0; i < 8; i++) {
                int d = digits_indices[i];

                // Set Color
                if (i < 2) printf(RED);       // Hours
                else if (i < 5) printf(GRN);  // Minutes
                else printf(BLU);             // Seconds

                // Print the slice
                printf("%s", digits[d][row]);
                printf(NOC);
            }
        }

        fflush(stdout); // Force output to screen

        if (total == 0) break;

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

    print_time_up_animation();
}

void print_small_timer(int hours, int minutes, int seconds) {
    int total = hours * 3600 + minutes * 60 + seconds;
    printf("Timer: \n");
    printf(RED "HH" NOC ":" GRN "MM" NOC ":" BLU "SS" NOC "\n");

    while (total >= 0) {
        printf("\r\033[K"); // Move start, Clear line
        printf(RED "%.2d" NOC ":" GRN "%.2d" NOC ":" BLU "%.2d" NOC , hours, minutes, seconds);
        fflush(stdout);

        if (total == 0) break;

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
        printf(BLD GRN "\rTIME UP!   " NOC);
        fflush(stdout);
        sleep(1);
        printf(BLD RED "\rTIME UP!   " NOC);
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
    exit(0);
}

int main(int argc, char **argv) {
    signal(SIGINT, cleanup_and_exit);

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    width = w.ws_col;
    height = w.ws_row;

    printf(HIDE_CURSOR); // Make it look cleaner

    int hours = 0, minutes = 0, seconds = 0;
    int set_big_timer = 1;

    if (argc < 2) {
        printf(SHOW_CURSOR); // Show cursor for input
        printf("Enter hours: ");
        scanf("%d", &hours);
        printf("Enter minutes: ");
        scanf("%d", &minutes);
        printf("Enter seconds: ");
        scanf("%d", &seconds);
        printf(HIDE_CURSOR); // Hide again
    }
    else {
        for (int i = 1; i < argc; i++) {
            if (!strncmp("-h", argv[i], 2) || !strncmp("--hours", argv[i], 7)) {
                if (i + 1 < argc) hours = atoi(argv[i+1]);
            }
            else if (!strncmp("-m", argv[i], 2) || !strncmp("--minutes", argv[i], 9)) {
                if (i + 1 < argc) {
                    minutes = atoi(argv[i+1]);
                    if (minutes >= 60) minutes = 59;
                }
            }
            else if (!strncmp("-s", argv[i], 2) || !strncmp("--seconds", argv[i], 9)) {
                if (i + 1 < argc) {
                    seconds = atoi(argv[i+1]);
                    if (seconds >= 60) seconds = 59;
                }
            }
            else if (!strncmp("-n", argv[i], 2) || !strncmp("--no-big", argv[i], 8)) {
                set_big_timer = 0;
            }
            else if (!strncmp("--help", argv[i], 6)) {
                usage(argv[0]);
            }
        }
    }

    if (hours == 0 && minutes == 0 && seconds == 0) {
        cleanup_and_exit(0);
    }

    if (set_big_timer) {
        print_big_timer(hours, minutes, seconds);
    }
    else {
        print_small_timer(hours, minutes, seconds);
    }

    cleanup_and_exit(0);
    return 0;
}
