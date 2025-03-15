#ifndef HELPER_H
#define HELPER_H

#include <time.h>

// Function to get the current date as a string
int get_current_date_str(char *buffer, size_t size);
// Function to get the current date as a struct tm
int get_current_date(struct tm *date);
// Function to convert a date string to a struct tm
int convert_str_to_date(const char* datestr, struct tm *date);
// Function to calculate the number of days between two dates
int days_between_dates(struct tm *date1, struct tm *date2);

#endif // HELPER_H