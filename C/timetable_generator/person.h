#ifndef PERSON_H
#define PERSON_H

#include <stdio.h>
#include <string.h>

typedef struct {
    char name[32];
    int available_days_hours[5][9];
    char available_lang[10][3];
    int lang_count;
} Person;

void print_person(Person person);
Person make_person();

#endif
