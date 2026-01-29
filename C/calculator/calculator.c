#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float evaluate(char *num1, char operand, char *num2) {
    if(operand == '*') {
        return atof(num1) * atof(num2);
    }
    else if(operand == '/') {
        return atof(num1) / atof(num2);
    }
    else if(operand == '+') {
        return atof(num1) + atof(num2);
    }
    else {
        return atof(num1) - atof(num2);
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

    float result;
    for (int j = 0; j < i; j++) {
        if(!strncmp(tokens[j], "*", 1)
        || !strncmp(tokens[j], "/", 1)
        || !strncmp(tokens[j], "+", 1)
        || !strncmp(tokens[j], "-", 1))
        {
            result = evaluate(tokens[j-1], tokens[j][0], tokens[j+1]);
        }
    }

    printf("result: %.2f\n", result);

    return 0;
}
