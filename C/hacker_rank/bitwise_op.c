#include <stdio.h>

void calculate_the_maximum(int n, int k) {
    int and = 0, or = 0, xor = 0;
  //Write your code here.
    for(int i = 1; i < n; i++) {
        for(int j = i+1; j <= n; j++) {
            int for_and = i & j;
            if(for_and < k && for_and > and) {
                and = for_and;
            }
            int for_or = i | j;
            if(for_or < k && for_or > or) {
                or = for_or;
            }
            int for_xor = i ^ j;
            if(for_xor < k && for_xor > xor) {
                xor = for_xor;
            }
        }
    }
    printf("%d\n", and);
    printf("%d\n", or);
    printf("%d\n", xor);
}

int main() {
    int n, k;

    scanf("%d %d", &n, &k);
    calculate_the_maximum(n, k);

    return 0;
}

