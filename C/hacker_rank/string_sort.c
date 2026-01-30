#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lexicographic_sort(const char* a, const char* b) {
    return strcmp(a, b);
}

int lexicographic_sort_reverse(const char* a, const char* b) {
    return strcmp(b, a);
}

int distinct(const char *a) {
    int alphabet[26] = {0};
    int distinct = 0;
    for(int i = 0; a[i] != '\0'; i++) {
        int idx = a[i] - 'a';
        if(alphabet[idx] == 0) {
            distinct++;
            alphabet[idx] = 1;
        }
    }
    return distinct;
}

int sort_by_number_of_distinct_characters(const char* a, const char* b) {
    int a_distinct = distinct(a);
    int b_distinct = distinct(b);

    int diff = a_distinct - b_distinct;
    if (diff == 0) {
        return lexicographic_sort(a, b);
    }

    return a_distinct - b_distinct;
}

int sort_by_length(const char* a, const char* b) {
    if(strlen(a) == strlen(b)) {
        return lexicographic_sort(a, b);
    }
    return strlen(a) - strlen(b);
}

void string_sort(char** arr,const int len,int (*cmp_func)(const char* a, const char* b)){
    for(int i = 0; i < len-1; i++) {
        for(int j = i+1; j < len; j++) {
            if(cmp_func(arr[i], arr[j]) > 0) {
                char *temp = arr[j];
                arr[j] = arr[i];
                arr[i] = temp;
            }
        }
    }
}


int main()
{
    int n;
    scanf("%d", &n);

    char** arr;
	arr = (char**)malloc(n * sizeof(char*));

    for(int i = 0; i < n; i++){
        *(arr + i) = malloc(1024 * sizeof(char));
        scanf("%s", *(arr + i));
        *(arr + i) = realloc(*(arr + i), strlen(*(arr + i)) + 1);
    }

    string_sort(arr, n, lexicographic_sort);
    for(int i = 0; i < n; i++)
        printf("%s\n", arr[i]);
    printf("\n");

    string_sort(arr, n, lexicographic_sort_reverse);
    for(int i = 0; i < n; i++)
        printf("%s\n", arr[i]);
    printf("\n");

    string_sort(arr, n, sort_by_length);
    for(int i = 0; i < n; i++)
        printf("%s\n", arr[i]);
    printf("\n");

    string_sort(arr, n, sort_by_number_of_distinct_characters);
    for(int i = 0; i < n; i++)
        printf("%s\n", arr[i]);
    printf("\n");
}
