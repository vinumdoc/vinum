#include "library_api.h"
#include <stdio.h>

char* day_of_week(char* arg) {
    char* days_of_week[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };

    int year, month, day;

    if (sscanf(arg, "%d-%d-%d", &year, &month, &day) != 3) {
        return "[ERROR: Invalid date format]";
    }

    // Zeller algorithm to calculate the day of the week
    if (month < 3) {
        month += 12;
        year -= 1;
    }

    int k = year % 100;
    int j = year / 100;
    int h = (day + (13 * (month + 1)) / 5 + k + k / 4 + j / 4 - 2 * j) % 7;

    int day_of_week = (h + 5) % 7;

    return days_of_week[day_of_week];
}

DEFINE_LIBRARY(
    {"day_of_week", day_of_week}
    )
