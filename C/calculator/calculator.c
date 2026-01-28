#include <stdio.h>

int main() {
    char expression[128];
    printf("Enter an expression: ");
    fgets(expression, 127, stdin);
    printf("Your expression: %s\n", expression);

    return 0;
}
