#include <stdio.h>
#include <string.h>

// A hardcoded "Rotor" - a random permutation of 0-255
// In a real scenario, this table would be derived from your key,
// but for this "Enigma" mimic, we use a fixed wiring.
const unsigned char ROTOR[256] = {
    0xA3, 0x12, 0x7F, 0x45, 0x98, 0xCD, 0x56, 0xB2, 0x01, 0xFE, 0x88, 0x24, 0x6A, 0x91, 0x33, 0xE0,
    0x17, 0x55, 0x22, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x02, 0x03, 0x04, 0x05, 0x06,
    // ... (To save space, I am generating the rest programmatically below.
    // In a real Enigma, this is a physical wire connection.)
};

// We will generate a full 0-255 S-Box (Substitution Box) on the fly for simplicity
unsigned char SBOX[256];
unsigned char INV_SBOX[256];

void initialize_rotors(const char *key) {
    // 1. Initialize SBOX simply with 0..255
    for (int i = 0; i < 256; i++) {
        SBOX[i] = i;
    }

    // 2. Scramble SBOX based on the user's Key (This mimics the "Plugboard" settings)
    int len = strlen(key);
    int j = 0;
    for (int i = 0; i < 256; i++) {
        j = (j + SBOX[i] + key[i % len]) % 256;

        // Swap
        unsigned char temp = SBOX[i];
        SBOX[i] = SBOX[j];
        SBOX[j] = temp;
    }

    // 3. Create the Inverse SBOX for decryption
    for (int i = 0; i < 256; i++) {
        INV_SBOX[SBOX[i]] = i;
    }
}

void process_file(const char *input_path, const char *output_path, int mode) {
    FILE *fin = fopen(input_path, "rb");
    FILE *fout = fopen(output_path, "wb");

    if (!fin || !fout) {
        printf("Error opening files.\n");
        return;
    }

    int c;
    unsigned int tick = 0; // The "rotation" counter

    // Read byte by byte
    while ((c = fgetc(fin)) != EOF) {
        unsigned char ch = (unsigned char)c;
        unsigned char processed;

        // "Rotate" the logic based on the tick counter
        // This ensures 'A' encrypts differently every time it appears

        if (mode == 1) { // Encrypt
            // 1. Shift input by tick
            unsigned char input_shifted = (ch + tick) % 256;
            // 2. Pass through Rotor (SBOX)
            processed = SBOX[input_shifted];
        } else { // Decrypt
            // 1. Reverse pass through Rotor (INV_SBOX)
            unsigned char reversed = INV_SBOX[ch];
            // 2. Un-shift by tick (handling negative modulo)
            int val = reversed - (tick % 256);
            if (val < 0) val += 256;
            processed = (unsigned char)val;
        }

        fputc(processed, fout);

        // Rotate the machine for the next character
        tick++;
    }

    fclose(fin);
    fclose(fout);
    printf("Operation complete: %s -> %s\n", input_path, output_path);
}

int main(int argc, char *argv[]) {
    // for(int i = 0; i < argc; i++) {
    //     printf("%d\t%s\n", i+1, argv[i]);
    // }
    if (argc != 7) {
        printf("Usage:\n");
        printf("  %s --encrypt <in> -o <out> --key <key>\n", argv[0]);
        printf("  %s --decrypt <in> -o <out> --key <key>\n", argv[0]);
        return 1;
    }

    const char *mode_str = argv[1];
    const char *input_file = argv[2];
    const char *output_file = argv[4];
    const char *key = argv[5];

    // Setup the "Machine"
    initialize_rotors(key);

    if (strcmp(mode_str, "--encrypt") == 0) {
        process_file(input_file, output_file, 1);
    } else if (strcmp(mode_str, "--decrypt") == 0) {
        process_file(input_file, output_file, 0);
    } else {
        printf("Unknown mode: %s\n", mode_str);
        return 1;
    }

    return 0;
}
