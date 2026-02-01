#include <stdio.h>

int main() {
    int n;
    scanf("%d", &n);

    int border = n * 2 - 1;
    for(int i = 0; i < border; i++) {
        for(int j = 0; j < border; j++) {
            int min_i = i < border-1-i ? i : border-1-i;
            int min_j = j < border-1-j ? j : border-1-j;
            int min_dis = min_i < min_j ? min_i : min_j;
            printf("%d ", n - min_dis);
        }
        printf("\n");
    }

    return 0;
}
