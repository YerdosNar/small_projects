#include "person.h"

// Helper function to clear the input buffer so scanf and fgets play nice
void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_person(Person person) {
    printf("\nNAME: %s\n", person.name);

    printf("AVAILABLE LANGS: { ");
    for (int i = 0; i < person.lang_count; i++) {
        printf("%s", person.available_lang[i]);
        if (i < person.lang_count - 1) printf(", ");
    }
    printf(" }\n");

    printf("AVAILABLE DAYS and HOURS: {\n");
    for (int i = 0; i < 5; i++) {
        printf("    DAY %d: { ", i);
        for (int j = 0; j < 9; j++) {
            if (person.available_days_hours[i][j] == 1) {
                printf("%d:00 ", j + 9); // Print the actual hour they are free
            }
        }
        printf("}\n");
    }
    printf("}\n");
}

Person make_person() {
    Person person;

    // Initialize array to 0 (unavailable)
    memset(person.available_days_hours, 0, sizeof(person.available_days_hours));

    printf("\nEnter person's name: ");
    fgets(person.name, 31, stdin);
    person.name[strcspn(person.name, "\n")] = 0; // Remove trailing newline

    printf("How many languages does %s speak? (Max 10): ", person.name);
    scanf("%d", &person.lang_count);
    if (person.lang_count > 10) person.lang_count = 10;

    printf("Enter languages (2-letter codes like EN, SP, FR):\n");
    for (int i = 0; i < person.lang_count; i++) {
        printf("  Language %d: ", i + 1);
        scanf("%2s", person.available_lang[i]);
    }

    printf("\nEnter availability (1 = Available, 0 = Not Available)\n");
    const char *days[] = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY"};

    for (int i = 0; i < 5; i++) {
        int is_available;
        printf("Is %s available at all on %s? (1/0): ", person.name, days[i]);
        scanf("%d", &is_available);

        if (!is_available) continue; // Skip to next day if not available at all

        printf("  Enter 1 or 0 for each hour (9 to 17):\n");
        for (int j = 0; j < 9; j++) {
            printf("    %02d:00 -> ", j + 9);
            scanf("%d", &person.available_days_hours[i][j]);
        }
    }
    clear_buffer(); // Clean up for the next person's name input
    return person;
}
