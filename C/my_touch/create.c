#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long Ul;

// Bytes
#define KB 1024
#define MB (KB * 1024)
#define GB (MB * 1024)
Ul TB = (Ul)GB * 1024;

// bits
#define Kb KB / 8
#define Mb Kb * 1024
#define Gb Mb * 1024
Ul Tb = (Ul)Gb * 1024;

int main(int argc, char **argv) {
    // printf("KB %d\n", KB);
    // printf("MB %d\n", MB);
    // printf("GB %d\n", GB);
    // printf("TB %lu\n", TB);

    char filename[128];
    int file_size;
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
        scanf("%d", &file_size);
        getchar();

        printf("Enter size format: ");
        if (fgets(size_form, 7, stdin)) {
            size_form[strcspn(size_form, "\n")] = 0;
        }
    }
    else {
        for (int i = 0; i < argc; i++) {
            if ((!strncmp(argv[i], "-s", 2) || !strncmp(argv[i], "--size", 6)) && i+2 < argc) {
                file_size = atoi(argv[i+1]);
                strncpy(argv[i+2], size_form, 7);
            }
        }
    }

    char fullname[161];
    snprintf(fullname, 160, "%s.%s", filename, extension);

    printf("Creating a file '%s' size=%d%s\n", fullname, file_size, size_form);

    FILE *fp = fopen(fullname, "wb");
    if (!fp) {
        fprintf(stderr, "ERROR: Could not open '%s'\n", filename);
        return 1;
    }

    if (!strncmp(size_form, "K", 1) || !strncmp(size_form, "KB", 2)) {
        for (size_t i = 0; i < KB; i++) {
            for (int j = 0; j < file_size; j++) {
                fwrite("a", 1, 1, fp);
            }
            fwrite("\n", 1, 1, fp);
        }
    }
    else if (!strncmp(size_form, "M", 1) || !strncmp(size_form, "MB", 2)) {
        for (size_t i = 0; i < MB; i++) {
            for (int j = 0; j < file_size; j++) {
                fwrite("a", 1, 1, fp);
            }
            fwrite("\n", 1, 1, fp);
        }
    }
    else if(!strncmp(size_form, "G", 1) || !strncmp(size_form, "GB", 2)) {
        for (size_t i = 0; i < GB; i++) {
            for (int j = 0; j < file_size; j++) {
                fwrite("a", 1, 1, fp);
            }
            fwrite("\n", 1, 1, fp);
        }
    }
    fclose(fp);
}
