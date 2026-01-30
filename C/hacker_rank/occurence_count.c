#include <stdio.h>
#include <string.h>

int main() {

    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    char str[1024];
    int occurence[10] = {0};
    fgets(str, 1023, stdin);
    for(size_t i = 0; i < strlen(str); i++) {
        if(str[i] < 48 || str[i] > 57) {
            continue;
        }
        else {
            int idx = (int)str[i] - 48;
            occurence[idx]++;
        }
    }
    for(int i = 0; i < 10; i++) {
        printf("%d ", occurence[i]);
    }
    return 0;
}

