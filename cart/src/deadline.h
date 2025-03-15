#ifndef DEADLINE_H
#define DEADLINE_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// Function to handle the 'deadline' command
int cmd_deadline(int argc, char *argv[]);
// Function to handle the 'deadline set' command
int cmd_deadline_set(int argc, char *argv[]);
// Function to handle the 'deadline get' command
int cmd_deadline_get(int argc, char *argv[]);
// Function to handle the 'deadline check' command
int cmd_deadline_check(int argc, char *argv[]);

// Date helper functions used by tests
int get_current_date_str(char *date_str, size_t size);
int get_current_date(struct tm *date);
int convert_str_to_date(const char *date_str, struct tm *date);
int days_between_dates(struct tm *date1, struct tm *date2);

#ifdef __cplusplus
}
#endif

#endif // DEADLINE_H