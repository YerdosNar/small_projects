#include <ncurses.h>

int main() {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(0);

        int height = 10, width = 40;
        int start_y = (LINES - height) / 2;
        int start_x = (COLS - width) / 2;

        WINDOW *win = newwin(height, width, start_y, start_x);
        refresh();
        box(win, 0, 0);

        mvwprintw(win, 2, 2, "C TUI DEMO");
        mvwprintw(win, 4, 2, "Press 'q' to exit");
        mvwprintw(win, 5, 2, "Press 'a' for a surprise");
        wrefresh(win);

        int ch;
        while ((ch = wgetch(win)) != 'q') {
                switch(ch) {
                        case 'a':
                                mvwprintw(win, 7, 2, "You pressed 'a'!  ");
                                break;
                        default:
                                mvwprintw(win, 7, 2, "Key code: %d      ", ch);
                                break;
                }
                wrefresh(win);
        }
        endwin();

        return 0;
}
