#include <stdio.h>
#include <string.h>
#include "person.h"

#define MAX_PEOPLE 10

// Checks if two people share at least one language
int share_language(Person p1, Person p2) {
    for (int i = 0; i < p1.lang_count; i++) {
        for (int j = 0; j < p2.lang_count; j++) {
            if (strcmp(p1.available_lang[i], p2.available_lang[j]) == 0) {
                return 1; // Match found!
            }
        }
    }
    return 0; // No shared language
}

void generate_timetable(Person people[], int total_people) {
    printf("\n=====================================\n");
    printf("         WEEKLY TIMETABLE\n");
    printf("=====================================\n");

    const char *days[] = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY"};

    for (int day = 0; day < 5; day++) {
        printf("\n--- %s ---\n", days[day]);

        for (int hour = 0; hour < 9; hour++) {
            printf("  %02d:00 - %02d:00 : ", hour + 9, hour + 10);

            // Array to keep track of who is already paired up in this time slot
            int paired[MAX_PEOPLE] = {0};
            int pairs_found = 0;

            for (int i = 0; i < total_people; i++) {
                // If Person 'i' is free and not already paired in this hour
                if (people[i].available_days_hours[day][hour] == 1 && !paired[i]) {

                    // Look for a partner 'j'
                    for (int j = i + 1; j < total_people; j++) {
                        if (people[j].available_days_hours[day][hour] == 1 && !paired[j]) {

                            if (share_language(people[i], people[j])) {
                                printf("[%s & %s]  ", people[i].name, people[j].name);
                                paired[i] = 1; // Mark them as busy for this hour
                                paired[j] = 1;
                                pairs_found++;
                                break; // Move to the next available person 'i'
                            }
                        }
                    }
                }
            }
            if (pairs_found == 0) {
                printf("No matching couples available.");
            }
            printf("\n");
        }
    }
}

int main() {
    Person group[MAX_PEOPLE];
    int num_people;

    printf("How many people are we scheduling? (Max %d): ", MAX_PEOPLE);
    scanf("%d", &num_people);

    // Safety check
    if (num_people > MAX_PEOPLE) num_people = MAX_PEOPLE;

    // Clear buffer before we start hitting fgets in make_person
    int c; while ((c = getchar()) != '\n' && c != EOF);

    // Gather input
    for (int i = 0; i < num_people; i++) {
        printf("\n--- Entering details for Person %d ---", i + 1);
        group[i] = make_person();
    }

    // Generate and print the timetable
    generate_timetable(group, num_people);

    return 0;
}
