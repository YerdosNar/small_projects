#include <stdio.h>
#include <string.h>

int find_sign(char *string, char sign) {
    for(int i = 0; i < strlen(string); i++) {
        if (string[i] == sign) {
            return i;
        }
    }
    return -1;
}

// void get_substring(char *string, char *sub_string, int from, int to) {
//     int i = 0;
//     while(from+i < to) {
//         sub_string[i] = string[from+i];
//         printf("from get_substring: %s\n", sub_string);
//         i++;
//     }
// }
// void tokenize(char *str, char **tokens) {
//     int start = 0, end = 0, idx = 0;
//     for(int i = 0; i < strlen(str); i++) {
//         if(str[i] == ' '
//         || str[i] == '*'
//         || str[i] == '/'
//         || str[i] == '+'
//         || str[i] == '-'
//         || str[i] == '%'
//         || str[i] == ')'
//         || str[i] == '(') {
//             get_substring(str, tokens[idx++], start, i);
//             start = i;
//         }
//         i++;
//     }
// }

int main() {
    char expression[128];
    printf("Enter an expression: ");
    fgets(expression, 127, stdin);

    printf("%d\n", find_sign(expression, '*'));
    char *tokens[64];
    int i = 0;
    char *token = strtok(expression, " ");
    while(token != NULL && i < 63) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    printf("Your expression: \n");
    for(int j = 0; j < 64; j++) {
        printf("%s\n", tokens[j]);
    }


    return 0;
}
