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

// Cursor controls
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
#define CLEAR_SCREEN "\033[2J\033[H"
#define HOME_CURSOR "\033[H"

// BLOCK FONT: DIGITS 0-9 and Colon
const char *digits[11][5] = {
    // 0
    {"  ###  ", " #   # ", " #   # ", " #   # ", "  ###  "},
    // 1
    {"   #   ", "  ##   ", "   #   ", "   #   ", "  ###  "},
    // 2
    {"  ###  ", " #   # ", "    #  ", "   #   ", " ##### "},
    // 3
    {"  ###  ", " #   # ", "   ##  ", " #   # ", "  ###  "},
    // 4
    {"   #   ", "  ##   ", " # #   ", " ##### ", "   #   "},
    // 5
    {" ##### ", " #     ", " ##### ", "     # ", " ##### "},
    // 6
    {"  ###  ", " #     ", " ####  ", " #   # ", "  ###  "},
    // 7
    {" ##### ", "     # ", "    #  ", "   #   ", "   #   "},
    // 8
    {"  ###  ", " #   # ", "  ###  ", " #   # ", "  ###  "},
    // 9
    {"  ###  ", " #   # ", "  #### ", "     # ", "  ###  "},
    // : (Colon - Index 10)
    {"       ", "   ##  ", "       ", "   ##  ", "       "}
};

// BLOCK FONT: T, I, M, E, U, P, !
// Indices: 0=T, 1=I, 2=M, 3=E, 4=U, 5=P, 6=!, 7=Space
const char *letters[8][5] = {
    // T (0)
    {" ##### ", "   #   ", "   #   ", "   #   ", "   #   "},
    // I (1)
    {" ##### ", "   #   ", "   #   ", "   #   ", " ##### "},
    // M (2)
    {" #   # ", " ## ## ", " # # # ", " #   # ", " #   # "},
    // E (3)
    {" ##### ", " #     ", " ####  ", " #     ", " ##### "},
    // U (4)
    {" #   # ", " #   # ", " #   # ", " #   # ", "  ###  "},
    // P (5)
    {" ####  ", " #   # ", " ####  ", " #     ", " #     "},
    // ! (6)
    {"   #   ", "   #   ", "   #   ", "       ", "   #   "},
    // Space (7)
    {"       ", "       ", "       ", "       ", "       "}
};

void restore_terminal() {
    printf(SHOW_CURSOR);
    printf("\n");
    system("stty echo"); // Ensure echo is back on
}

void signal_handler(int sig) {
    restore_terminal();
    exit(sig);
}

// Helper to center output based on screen size
void move_to_center(int row_offset, int width_of_block, int screen_h, int screen_w) {
    int y = (screen_h - 5) / 2 + row_offset; // 5 is height of block
    int x = (screen_w - width_of_block) / 2;
    if (y < 0) y = 0;
    if (x < 0) x = 0;
    printf("\033[%d;%dH", y, x);
}

void print_time_up_big(int screen_h, int screen_w) {
    // "TIME UP!" indices in letters array
    // T, I, M, E, Space, U, P, !
    int msg_indices[] = {0, 1, 2, 3, 7, 4, 5, 6};
    int msg_len = 8;
    int total_width = msg_len * 7; // Approx 7 chars per letter

    int color_toggle = 0;

    printf(CLEAR_SCREEN);

    while(1) {
        // Move to start position for the whole block
        // We calculate position once per frame, but we need to reset cursor for each ROW of the block
        int start_y = (screen_h - 5) / 2;
        int start_x = (screen_w - total_width) / 2;

        // Pick Color
        char *color = color_toggle ? BLD RED : BLD BLU;

        // Print 5 rows
        for (int row = 0; row < 5; row++) {
            printf("\033[%d;%dH", start_y + row, start_x); // Move cursor to start of this row

            for (int i = 0; i < msg_len; i++) {
                printf("%s%s" NOC, color, letters[msg_indices[i]][row]);
            }
        }

        printf(NOC);
        fflush(stdout);

        color_toggle = !color_toggle;

        // Simple beep (optional)
        // printf("\a");

        sleep(1);
    }
}

void print_big_timer(int h, int m, int s, int screen_h, int screen_w) {
    long total_seconds = h * 3600 + m * 60 + s;

    // Width calculation: 6 digits + 2 colons. Each approx 7 chars wide.
    // 8 blocks * 7 chars = 56 chars wide
    int block_width = 56;

    printf(CLEAR_SCREEN);

    while (total_seconds >= 0) {
        // Recalculate H, M, S from total seconds
        int ch = total_seconds / 3600;
        int rem = total_seconds % 3600;
        int cm = rem / 60;
        int cs = rem % 60;

        // Prepare indices for HH:MM:SS
        int indices[] = {
            ch / 10, ch % 10,
            10, // :
            cm / 10, cm % 10,
            10, // :
            cs / 10, cs % 10
        };

        int start_y = (screen_h - 5) / 2;
        int start_x = (screen_w - block_width) / 2;

        // Print the 5 rows
        for (int row = 0; row < 5; row++) {
            printf("\033[%d;%dH", start_y + row, start_x);
            for (int i = 0; i < 8; i++) {
                // Color Logic
                if (i < 2) printf(RED);      // Hours
                else if (i < 5) printf(GRN); // Minutes
                else printf(BLU);            // Seconds

                printf("%s" NOC, digits[indices[i]][row]);
            }
        }

        fflush(stdout);

        if (total_seconds == 0) break;

        total_seconds--;
        sleep(1);
    }

    // Trigger Time Up Animation
    print_time_up_big(screen_h, screen_w);
}

void print_small_timer(int hours, int minutes, int seconds) {
    int total = hours * 3600 + minutes * 60 + seconds;
    printf("\nTimer Started:\n");

    while (total >= 0) {
        printf("\r\033[K"); // Clear line
        printf(BLD RED "%.2d" NOC ":" BLD GRN "%.2d" NOC ":" BLD BLU "%.2d" NOC, hours, minutes, seconds);
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

    printf("\n\n");
    while(1) {
        printf(BLD RED "\rTIME UP!   " NOC);
        fflush(stdout);
        sleep(1);
        printf(BLD YEL "\rTIME UP!   " NOC);
        fflush(stdout);
        sleep(1);
    }
}

void usage(char *exe) {
    printf("Usage: %s [options]\n", exe);
    printf("Options: \n");
    printf("  -h, --hours <number>    Set hours\n");
    printf("  -m, --minutes <number>  Set minutes\n");
    printf("  -s, --seconds <number>  Set seconds\n");
    printf("  -n, --no-big            Use small text mode\n");
    exit(0);
}

int main(int argc, char **argv) {
    // Setup clean exit on Ctrl+C
    signal(SIGINT, signal_handler);

    // Hide Cursor immediately
    printf(HIDE_CURSOR);

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = w.ws_col;
    int height = w.ws_row;

    int hours = 0, minutes = 0, seconds = 0;
    int set_big_timer = 1;

    // Argument Parsing
    if (argc < 2) {
        // Need to show cursor for input
        printf(SHOW_CURSOR);
        printf("Enter hours: ");
        scanf("%d", &hours);
        printf("Enter minutes: ");
        scanf("%d", &minutes);
        printf("Enter seconds: ");
        scanf("%d", &seconds);
        printf(HIDE_CURSOR); // Hide again
    } else {
        for (int i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--hours")) {
                if (i + 1 < argc) hours = atoi(argv[++i]);
            }
            else if (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--minutes")) {
                if (i + 1 < argc) {
                    minutes = atoi(argv[++i]);
                    if (minutes >= 60) minutes = 59;
                }
            }
            else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--seconds")) {
                if (i + 1 < argc) {
                    seconds = atoi(argv[++i]);
                    if (seconds >= 60) seconds = 59;
                }
            }
            else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--no-big")) {
                set_big_timer = 0;
            }
            else if (!strcmp(argv[i], "--help")) {
                restore_terminal();
                usage(argv[0]);
            }
        }
    }

    if (hours == 0 && minutes == 0 && seconds == 0) {
        restore_terminal();
        fprintf(stderr, "ERROR: Timer set to 0.\n");
        return 1;
    }

    if (set_big_timer) {
        print_big_timer(hours, minutes, seconds, height, width);
    } else {
        print_small_timer(hours, minutes, seconds);
    }

    restore_terminal();
    return 0;
}
