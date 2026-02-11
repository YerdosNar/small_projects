#include <stdio.h>

#define MAXLINE 1000

int custom_getline(char line[], int maxline) {
    int c, i;
    for(i=0; i < maxline - 1 && (c=getchar()) != EOF && c != '\n'; i++) {
        line[i] = c;
    }
    if (c == '\n') {
        line[i++] = c;
    }
    line[i] = '\0';
    return i;
}

void copy(char to[], char from[]) {
    int i = 0;
    while((to[i] = from[i]) != '\0') {
        i++;
    }
}

int main(void) {
    int len, max=0;

    char line[MAXLINE], longest[MAXLINE];
    printf("Enter: ");
    while((len=custom_getline(line, MAXLINE)) > 0) {
        if(len > max) {
            max=len;
            copy(longest, line);
        }
        printf("Enter: ");
    }
    if(max > 0) {
        printf("\nLongest: %s\n", longest);
        printf("Length: %d\n", max);
    }

    return 0;
}
