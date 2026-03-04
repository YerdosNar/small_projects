#include <stdio.h>
#include <string.h>
#include "person.h"

#define MAX_PEOPLE 10
extern const char *days[];

// New structure to hold workers per hour
typedef struct {
    int assigned_count;
    char worker_names[4][32]; // Max 4 people per hour
} hour_slot;

typedef struct {
    char day_name[4];
    hour_slot hour[9];
} day_element;

typedef struct {
    day_element day[5];
} timetable_t;

timetable_t schedule(Person people[], int total_people) {
    timetable_t result;

    for (int d = 0; d < 5; d++) {
        strncpy(result.day[d].day_name, days[d], 3);
        result.day[d].day_name[3] = '\0';

        for (int h = 0; h < 9; h++) {
            result.day[d].hour[h].assigned_count = 0;

            // Find up to 4 available people for this hour
            for (int i = 0; i < total_people; i++) {
                if (people[i].available_days_hours[d][h] == 1) {
                    int count = result.day[d].hour[h].assigned_count;

                    if (count < 4) { // We have room for them
                        strncpy(result.day[d].hour[h].worker_names[count], people[i].name, 31);
                        result.day[d].hour[h].assigned_count++;
                    }
                }
            }
        }
    }
    return result;
}

void print_timetable(timetable_t t) {
    printf("\n+========================================================================================+\n");
    printf("|    Time     |");
    for (int d = 0; d < 5; d++) {
        printf("     %-5s    |", t.day[d].day_name);
    }
    printf("\n+========================================================================================+\n");

    for (int h = 0; h < 9; h++) {
        printf("| %02d:00-%02d:00 |", h + 9, h + 10);

        for (int d = 0; d < 5; d++) {
            int count = t.day[d].hour[h].assigned_count;

            if (count == 0) {
                printf(" %-12s |", "--");
            } else if (count == 1) {
                printf(" %-12.12s |", t.day[d].hour[h].worker_names[0]);
            } else if (count == 2) {
                // Shorten names to fit visually if there are 2 people
                printf(" %-4.4s & %-5.5s |", t.day[d].hour[h].worker_names[0], t.day[d].hour[h].worker_names[1]);
            } else {
                printf(" %d workers    |", count);
            }
        }
        printf("\n");
    }
    printf("+========================================================================================+\n");
}

int main() {
    Person group[MAX_PEOPLE];
    int num_people;

    printf("How many people? (Max %d): ", MAX_PEOPLE);
    if (scanf("%d", &num_people) != 1) return 1;

    if (num_people > MAX_PEOPLE) num_people = MAX_PEOPLE;

    // Clear buffer
    int c; while ((c=getchar()) != '\n' && c != EOF);

    for (int i = 0; i < num_people; i++) {
        printf("\n--- Person %d ---\n", (i+1));
        group[i] = make_person();
    }

    timetable_t final_schedule = schedule(group, num_people);
    print_timetable(final_schedule);

    return 0;
}
