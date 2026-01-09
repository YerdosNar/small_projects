#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define RED "\033[31m"
#define GRN "\033[32m"
#define YEL "\033[33m"
#define BLU "\033[34m"
#define CYN "\033[36m"

#define BLD "\033[1m"
#define NOC "\033[0m"

typedef struct {
    int length;
    int write;
    char *password;
    char *filename;
} Options;

Options opts = {
    .length = 15,
    .write = 0,
    .password = NULL,
    .filename = NULL
};

void parse_args(int argc, char **argv, Options *opts) {
    for (int i = 0; i < argc; i++) {
        if ((!strcmp(argv[i], "-l") || !strcmp(argv[i], "--length")) && i + 1 < argc) {
            opts->length = atoi(argv[i+1]);
            if (opts->length > 80) {
                fprintf(stderr, BLD RED "ERROR: Password length can be between 1~80.\n");
                printf(YEL "You set length: %d\n", opts->length);
                opts->length = 80;
                printf(YEL "Setting length: %d\n" NOC, opts->length);
            }
            else if (opts->length < 5) {
                fprintf(stderr, BLD RED "ERROR: Password length can be between 1~80.\n");
                printf(YEL "You set length: %d\n", opts->length);
                opts->length = 5;
                printf(YEL "Setting length: %d\n" NOC, opts->length);
            }
        }
        else if (!strcmp(argv[i], "-w") || !strcmp(argv[i], "--write")) {
            opts->write = 1;
            if (i + 1 >= argc) {
                printf(YEL BLD "WARNING: File is not specified\nDefault: PasswordFile.txt\n" NOC);
                opts->filename = "PasswordFile.txt";
            }
            else {
                opts->filename = argv[i+1];
            }
        }
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
            printf("Usage: %s [options]\n", argv[0]);
            printf("\n");
            printf("Options:\n");
            printf("    -l/--length <number>        set length of the password [default=15].\n");
            printf("    -l/--write <filename>       save password to a file [default=PasswordFile.txt]\n");
            printf("\n");
            printf("Example:\n");
            printf("    %s -l 40 -w my_password.txt\n", argv[0]);
            printf("Generates password of length 40, and saves to 'my_password.txt' file\n");
            exit(0);
        }
    }
}

void print_value(Options opts) {
    printf(BLD CYN "\n+====================" NOC);
    printf(BLU BLD "\n| Password Length  : " NOC GRN "%d", opts.length);
    printf(BLU BLD "\n| Password         : " NOC GRN "%s", opts.password);
    if (opts.write) {
        printf(BLU BLD "\n| Password Filename: " GRN "%s", opts.filename);
    }
    printf(BLD CYN "\n+====================\n" NOC);
}

void generate(Options *opts) {
    char temp_pw[opts->length + 1];

    int choose = rand() % 2;
    if (choose) {
        temp_pw[0] = rand() % 26 + 65;
    }
    else {
        temp_pw[0] = rand() % 26 + 97;
    }

    for (int i = 1; i < opts->length; i++) {
        choose = rand() % 3;
        if (choose == 0) {
            temp_pw[i] = rand() % 10 + 48;
        }
        else if(choose == 1) {
            temp_pw[i] = rand() % 26 + 65;
        }
        else {
            temp_pw[i] = rand() % 26 + 97;
        }
    }

    temp_pw[opts->length] = '\0';
    opts->password = malloc(opts->length + 1);
    strcpy(opts->password, temp_pw);
}

void save_file(Options opts) {
    FILE *pw_file = fopen(opts.filename, "w");
    fwrite(opts.password, opts.length, 1, pw_file);
}

int main(int argc, char **argv) {
    srand(time(NULL));

    if (argc <= 1) {
        printf(YEL "Setting defaults\n" NOC);
    }
    else {
        parse_args(argc, argv, &opts);
    }

    generate(&opts);

    if (opts.write) {
        save_file(opts);
    }

    print_value(opts);
    printf("%d\n", argc);

    return 0;
}
