#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *min_max(char *s)
{
        if (!s) return NULL;

        char *tok = strtok(s, " ");
        int max = atoi(tok);
        int min = max;
        while (tok) {
                tok = strtok(NULL, " ");
                if (!tok) break;
                int n = atoi(tok);
                if (max < n) max = n;
                if (min > n) min = n;
        }

        // 11 bytes for min, 11 bytes for max, 1 byte for space, 1 byte for NUL
        char *result = malloc(24);
        if (!result) return NULL;
        snprintf(result, 24, "%d %d", min, max);
        return result;
}

int main(int argc, char **argv)
{
        char *result = min_max(argv[1]);
        printf("Result: %s\n", result);

        return 0;
}
