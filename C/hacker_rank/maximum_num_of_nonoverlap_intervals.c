#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();
char* ltrim(char*);
char* rtrim(char*);
char** split_string(char*);
int parse_int(char*);

/*
 * Complete the 'maximizeNonOverlappingMeetings' function below.
 *
 * The function is expected to return an INTEGER.
 * The function accepts 2D_INTEGER_ARRAY meetings as parameter.
 */

int maximizeNonOverlappingMeetings(int meetings_rows, int meetings_columns, int** meetings) {
    // sort
    for (int i = 0; i < meetings_rows-1; i++) {
        for (int j = i+1; j < meetings_rows; j++) {
            if (meetings[i][1] > meetings[j][1]) {
                int *temp = meetings[i];
                meetings[i] = meetings[j];
                meetings[j] = temp;
            }
        }
    }

    int count = 1;
    int endtime = meetings[0][1];
    for (int i = 1; i < meetings_rows; i++) {
        if (meetings[i][0] >= endtime) {
            endtime = meetings[i][1];
            count++;
        }
    }

    return count;
}
int main()
{
    int meetings_rows = parse_int(ltrim(rtrim(readline())));

    int meetings_columns = parse_int(ltrim(rtrim(readline())));

    int** meetings = malloc(meetings_rows * sizeof(int*));

    for (int i = 0; i < meetings_rows; i++) {
        *(meetings + i) = malloc(meetings_columns * (sizeof(int)));

        char** meetings_item_temp = split_string(rtrim(readline()));

        for (int j = 0; j < meetings_columns; j++) {
            int meetings_item = parse_int(*(meetings_item_temp + j));

            *(*(meetings + i) + j) = meetings_item;
        }
    }

    int result = maximizeNonOverlappingMeetings(meetings_rows, meetings_columns, meetings);

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

char* ltrim(char* str) {
    if (!str) {
        return '\0';
    }

    if (!*str) {
        return str;
    }

    while (*str != '\0' && isspace(*str)) {
        str++;
    }

    return str;
}

char* rtrim(char* str) {
    if (!str) {
        return '\0';
    }

    if (!*str) {
        return str;
    }

    char* end = str + strlen(str) - 1;

    while (end >= str && isspace(*end)) {
        end--;
    }

    *(end + 1) = '\0';

    return str;
}

char** split_string(char* str) {
    char** splits = NULL;
    char* token = strtok(str, " ");

    int spaces = 0;

    while (token) {
        splits = realloc(splits, sizeof(char*) * ++spaces);

        if (!splits) {
            return splits;
        }

        splits[spaces - 1] = token;

        token = strtok(NULL, " ");
    }

    return splits;
}

int parse_int(char* str) {
    char* endptr;
    int value = strtol(str, &endptr, 10);

    if (endptr == str || *endptr != '\0') {
        exit(EXIT_FAILURE);
    }

    return value;
}
