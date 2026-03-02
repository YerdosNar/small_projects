#include <ctype.h>
#include <stdio.h>
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

// buffer
#define BUFFER (8 * 1024)

Ul parse_size(const char* size_str) {
    Ul size = 0;
    char format = 'B'; // If nothing, then default=bytes

    if (sscanf(size_str, "%lu%c", &size, &format) >= 1) {
        format = toupper(format);
        switch (format) {
            case 'K': return size * KB;
            case 'M': return size * MB;
            case 'G': return size * GB;
            default: return size;
        }
    }

    // If nothing worked
    return 0;
}

void usage(const char* exec) {
    printf("Usage: %s <filename.ext> <size>[K|M|G]\n", exec);
    printf("Example: %s example.pdf 5M\n", exec);
}

int main(int argc, char **argv) {
    char filename[128];
    Ul file_size = 1024;
    int set_random = 0;

    if (argc == 1) {
        usage(argv[0]);
    }
    if (argc <= 5) {
        printf("Enter filename: ");
        if (fgets(filename, 127, stdin)) {
            filename[strcspn(filename, "\n")] = 0;
        }

        char size_str[64];
        printf("Enter file size: ");
        if (fgets(size_str, 63, stdin)) {
            size_str[strcspn(size_str, "\n")] = 0;
        }
        file_size = parse_size(size_str);
    }
    else {
        for (int i = 1; i < argc; i++) {
            if ((!strncmp("-f", argv[i], 2) || !strncmp("--file", argv[i], 6)) && i+1 < argc) {
                strncpy(filename, argv[++i], 127);
            }
            else if ((!strncmp("-s", argv[i], 2) || !strncmp("--size", argv[i], 6)) && i+1 < argc) {
                file_size = parse_size(argv[++i]);
            }
            else if (!strncmp("-r", argv[i], 2)) set_random = 1;
            else {fprintf(stderr, "ERROR: unknown arg '%s'\n", argv[i]);}
        }
    }

    printf("Creating file '%s' with size=%lu bytes\n", filename, file_size);

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "ERROR: Could not open '%s'\n", filename);
        return 1;
    }

    char buffer[BUFFER];
    FILE *src;
    if (!set_random) {
        src = fopen("/dev/zero", "rb");
        if (!src) { fprintf(stderr, "ERROR: Could not open '/dev/zero' for sourcing.\n");return 1;}
        printf("Filling with /dev/zero\n");
    }
    else {
        src = fopen("/dev/random", "rb");
        if (!src) { fprintf(stderr, "ERROR: Could not open '/dev/random' for sourcing.\n");return 1;}
        printf("Filling with /dev/random\n");
    }
    fread(buffer, BUFFER, 1, src);

    Ul bytes_written = 0;
    while (bytes_written < file_size) {
        Ul bytes_left = file_size - bytes_written;
        // If less than 8KB then write that size; else write full 8KB
        size_t chunk_size = (bytes_left < BUFFER) ? (size_t)bytes_left : BUFFER;

        size_t written = fwrite(buffer, 1, chunk_size, fp);
        if (written != chunk_size) {
            fprintf(stderr, "ERROR: I/O failed.\n");
            fprintf(stdout, "Wrote %lu bytes so far\n", bytes_written);
            fclose(fp);
            return 1;
        }
        bytes_written += chunk_size;
    }
    fclose(fp);
    printf("DONE!\n");

    return 0;
}
