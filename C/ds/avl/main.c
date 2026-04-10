#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct Node {
        int key;
        struct Node *left;
        struct Node *right;
        int height;
};

int max(int a, int b) {
        return (a > b) ? a : b;
}

int height(struct Node *N) {
        if (N == NULL) return 0;
        return N->height;
}

struct Node *newNode(int key) {
        struct Node *node = (struct Node *)malloc(sizeof(struct Node));
        if (!node) fprintf(stderr, "ERROR: malloc() failed.\n");
        node->key = key;
        node->left = NULL;
        node->right = NULL;
        node->height = 1;
        return (node);
}



int main() {
        return 0;
}
