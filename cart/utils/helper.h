#ifndef HELPER_H
#define HELPER_H

#include <time.h>

int get_current_date_str(char *buffer, size_t size);
int get_current_date(struct tm *date);
int convert_str_to_date(const char* datestr, struct tm *date);
int days_between_dates(struct tm *date1, struct tm *date2);
#endif // HELPER_H