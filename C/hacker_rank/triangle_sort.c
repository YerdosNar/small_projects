#include <stdio.h>
#include <stdlib.h>

struct triangle
{
	int a;
	int b;
	int c;
};

typedef struct triangle triangle;

float area(int a, int b, int c) {
    float p = (a+b+c) / 2.0;
    return p * (p - a) * (p - b) * (p - c);
}

void sort_by_area(triangle* tr, int n) {
	/**
	* Sort an array a of the length n
	*/
    float areas[n];
    for(int i = 0; i < n; i++) {
        areas[i] = area(tr[i].a, tr[i].b, tr[i].c);
    }

    int last_swap = n-1;
    for(int i = 1; i < n; i++) {
        int sorted = 1;
        int curr_swap = -1;
        for(int j = 0; j < last_swap; j++) {
            if(areas[i] > areas[j+1]) {
                float temp = areas[j];
                areas[j] = areas[j+1];
                areas[j+1] = temp;
                triangle tr_temp = tr[j];
                tr[j] = tr[j+1];
                tr[j] = tr_temp;
                sorted = 0;
                curr_swap = j;
            }
        }
        if(sorted) return;
        last_swap = curr_swap;
    }
}

int main()
{
	int n;
	scanf("%d", &n);
	triangle *tr = malloc(n * sizeof(triangle));
	for (int i = 0; i < n; i++) {
		scanf("%d%d%d", &tr[i].a, &tr[i].b, &tr[i].c);
	}
	sort_by_area(tr, n);
	for (int i = 0; i < n; i++) {
		printf("%d %d %d\n", tr[i].a, tr[i].b, tr[i].c);
	}
	return 0;
}
