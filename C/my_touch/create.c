#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    char filename[128];
    size_t file_size;
    char size_form[8];
    char extension[32];

    if (argc < 7) {
        printf("Enter filename: ");
        if (fgets(filename, 127, stdin)) {
            filename[strcspn(filename, "\n")] = 0;
        }

        printf("Enter file extension: ");
        if (fgets(extension, 31, stdin)) {
            extension[strcspn(extension, "\n")] = 0;
        }

        printf("Enter file size: ");
        scanf("%lu", &file_size);
        getchar();

        printf("Enter size format: ");
        if (fgets(size_form, 7, stdin)) {
            size_form[strcspn(size_form, "\n")] = 0;
        }
    }

    printf("Creating a file '%s.%s' size=%lu%s\n", filename, extension, file_size, size_form);
}
