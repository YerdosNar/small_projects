#include <stdio.h>

main() {
    int c;
    int blank = 0;
    while((c = getchar()) != EOF) {
        if(c == ' ') {
            blank++;
        }
    }
    printf("\nBlank: %d\n", blank);
}
