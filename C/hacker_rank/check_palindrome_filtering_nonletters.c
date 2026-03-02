#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();



/*
 * Complete the 'isAlphabeticPalindrome' function below.
 *
 * The function is expected to return a BOOLEAN.
 * The function accepts STRING code as parameter.
 */

bool isAlphabeticPalindrome(char* code) {
    int len = 0;
    char str[strlen(code)];

    for (int i = 0; i < (int)strlen(code); i++) {
        char c = code[i];
        if (c >= 65 && c <= 90) {
            str[len++] = (char)((int)c+32);
        }
        else if (c >= 97 && c <= 122) {
            str[len++] = c;
        }
    }

    for (int i = 0; i < len/2; i++) {
        if (str[i] != str[len-1-i]) return false;
    }

    return true;
}

int main()
{
    char* code = readline();
    bool result = isAlphabeticPalindrome(code);
    printf("%d\n", result);

    return 0;
}

char* readline() {
    size_t alloc_length = 1024;
    size_t data_length = 0;

    char* data = malloc(alloc_length);

    while (true) {
        char* cursor = data + data_length;
        char* line = fgets(cursor, alloc_length - data_length, stdin);

        if (!line) {
            break;
        }

        data_length += strlen(cursor);

        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n') {
            break;
        }

        alloc_length <<= 1;

        data = realloc(data, alloc_length);

        if (!data) {
            data = '\0';

            break;
        }
    }

    if (data[data_length - 1] == '\n') {
        data[data_length - 1] = '\0';

        data = realloc(data, data_length);

        if (!data) {
            data = '\0';
        }
    } else {
        data = realloc(data, data_length + 1);

        if (!data) {
            data = '\0';
        } else {
            data[data_length] = '\0';
        }
    }

    return data;
}
