#include <stdio.h>

#define MAX 1024

void reverse(char s[]) {
    int i = 0;
    while(s[i] != '\0') {
        i++;
    }

    for(int j = 0; j < i/2; j++) {
        char temp = s[j];
        s[j] = s[i-j-1];
        s[i-j-1] = temp;
    }
}

int custom_getline(char line[], int length) {
    int i = 0;
    char c;
    for(; i < length - 1 && (c=getchar()) != EOF && c != '\n'; ++i) {
        line[i] = c;
    }
    if(c == '\n') {
        line[i++] = c;
    }
    line[i] = '\0';

    return i;
}

int main(void) {
    int max = 0;
    int len;
    char line[MAX];

    while((len=custom_getline(line, MAX)) > 0) {
        reverse(line);

        printf("Reverse: %s\n", line);
        printf("Enter  : ");
    }

    return 0;
}
