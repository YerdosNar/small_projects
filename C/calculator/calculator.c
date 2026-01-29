#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float evaluate(float num1, char operand, float num2) {
    if(operand == '*') {
        return num1 * num2;
    }
    else if(operand == '/') {
        return num1 / num2;
    }
    else if(operand == '+') {
        return num1 + num2;
    }
    else {
        return num1 - num2;
    }
}

int main() {
    char expression[128];
    printf("Enter an expression: ");
    fgets(expression, 127, stdin);

    char *tokens[64];
    int i = 0;
    char *token = strtok(expression, " ");
    while(token != NULL && i < 63) {
        tokens[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    // 123 + 123 + 123
    float result = 0.0f;
    for (int j = 0; j < i; j++) {
        if(!strncmp(tokens[j], "*", 1)
        || !strncmp(tokens[j], "/", 1)
        || !strncmp(tokens[j], "+", 1)
        || !strncmp(tokens[j], "-", 1))
        {
            if (j == 1) {
                result += atof(tokens[j-1]);
            }
            result = evaluate(result, tokens[j][0], atof(tokens[j+1]));
        }
    }

    printf("result: %.2f\n", result);

    return 0;
}
