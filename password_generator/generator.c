#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int count=10;
    char name[64];
    char name_length = 0;
    if(argc < 2) {
        printf("Default values...\n");
    } else {
        for(int i = 1; i < argc; i++) {
            if(!strncmp("-c", argv[i], 2) || !strncmp("--count", argv[i], 7)) {
                if(i+1 < argc) {
                    count = atoi(argv[i+1])+1;
                    printf("count set to %d\n", count);
                    i += 2;
                    printf("i is: %d\n", i);
                } else {
                    fprintf(stderr, "ERROR: %s requires number.\n", argv[i]);
                    return 1;
                }
            }
            else if(!strncmp("-f", argv[i], 2) || !strncmp("--first-name", argv[i], 12)) {
                if(i+1 < argc) {
                    strncpy(name, argv[i+1], 63);
                    i += 2;
                    name_length = (char)(strnlen(name, 64));
                    count = count + name_length + 1;
                    printf("count set to %d\n", count);
                    printf("i is: %d\n", i);
                } else {
                    fprintf(stderr, "ERROR: %s requires a name.\n", argv[i]);
                    return 1;
                }
            }
        }
    }

    printf("Name: %s\nLength: %d\n", name, name_length);
    srand(time(NULL));
    char password[count];
    int choose = rand() % 2;
    if(choose) {
        password[0] = rand() % 26 + 65;
    } else {
        password[0] = rand() % 26 + 97;
    }
    for(int i = 1; i < count - name_length - 1; i++) {
        choose = rand() % 3;
        if(choose == 0) {
            password[i] = rand() % 10 + 48;
        } else if(choose == 1) {
            password[i] = rand() % 26 + 65;
        } else if(choose == 2) {
            password[i] = rand() % 26 + 97;
        }
    }
    for(int i = 0; i < name_length; i++) {
        int change_case = rand() % 2;
        if(change_case) {
            if(name[i] >= 97 && name[i] <= 122) {
                password[i + count - name_length - 1] = name[i] - 32;
            } else if(name[i] >= 65 && name[i] <= 90) {
                password[i + count - name_length - 1] = name[i] + 32;
            } else {
                fprintf(stderr, "ERROR: Not supported character: %c\n", name[i]);
            }
        } else {
            password[i + count - name_length - 1] = name[i];
        }
    }
    password[count-1] = '\0';
    printf("Your Password: %s\n", password);

    return 0;
}
