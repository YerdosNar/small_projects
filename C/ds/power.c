#include <stdio.h>
#include <time.h>

typedef long long LL;

double power_iter(double base, LL n) {
        // base=1
        double result = 1.0;

        // n = 100
        while (n > 0) {
                if (n % 2 == 1) {
                        result *= base;
                }

                base *= base;
                n /= 2;
        }

        return (n < 0) ? 1.0 / result : result;
}

void measure_time(double base, LL n) {
        clock_t start, end;
        double cpu_time_used;

        start = clock();
        double res = power_iter(base, n);
        end = clock();

        cpu_time_used = ((double)(end - start) / CLOCKS_PER_SEC);

        printf("n = %15lld | Result: %8.2f | Time: %f seconds\n", n, res, cpu_time_used);
}

int main() {
        double base = 1.00000001;
        LL nums[] = {10, 1e2, 1e3, 1e6, 1e9};

        printf("Power O(log n):\n");

        for (int i = 0; i < 5; i++) {
                measure_time(base, nums[i]);
        }

        return 0;
}
