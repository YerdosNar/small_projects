#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("Default values...\n");
    }


    srand(time(NULL));
    int count = atoi(argv[1]);

    char password[count];
    for(int i = 0; i < count-1; i++) {
        int choose = rand() % 3;
        if(choose == 0) {
            password[i] = rand() % 10 + 48;
        } else if(choose == 1) {
            password[i] = rand() % 26 + 65;
        } else if(choose == 2) {
            password[i] = rand() % 26 + 97;
        }
    }
    password[count-1] = '\0';
    printf("Your Password: %s\n", password);

    return 0;
}
