#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define RED "\033[31m"
#define GRN "\033[32m"
#define BLU "\033[34m"
#define BLD "\033[1m"
#define NOC "\033[0m"
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
// const char NUMBERS[10][10][13] = {
//     {
//         "+----------+\n",
//         "|**********|\n",
//         "|***+--+***|\n",
//         "|***+  +***|\n",
//         "|***+  +***|\n",
//         "|***+  +***|\n",
//         "|***+  +***|\n",
//         "|***+--+***|\n",
//         "|**********|\n",
//         "+----------+\n",
//     },
//     {
//         "+----------+\n",
//         "|**********|\n",
//         "+------+***|\n",
//         "       |***|\n",
//         "       |***|\n",
//         "       |***|\n",
//         "       |***|\n",
//         "       |***|\n",
//         "       |***|\n",
//         "       +---+\n"
//     },
// };

void print_digit(int num) {
    for (int i = 0; i < 5; i++) {
        printf("%s\n", digits[num][i]);
    }
}

int main() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = w.ws_col;
    int height = w.ws_row;
    int x_pixel = w.ws_xpixel;
    int y_pixel = w.ws_ypixel;

    int minutes, seconds;
    printf("Enter minutes: ");
    scanf("%d", &minutes);
    printf("Enter seconds: ");
    scanf("%d", &seconds);

    // for(int i = 0; i < 11; i++) {
    //     print_digit(i);
    //     printf("\n");
    // }

    printf("Hello, World!\n");

    return 0;
}
