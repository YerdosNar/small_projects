#include <stdio.h>

main() {
    printf("%d\n", EOF);
    int c;
    while((c = getchar()) != EOF) {
        putchar(c);
    }
}
