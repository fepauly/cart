#include "helper.h"

void get_current_date_str(char *buffer, size_t size) {
    const time_t now = time(NULL);

    struct tm tm_info = *localtime(&now);
    strftime(buffer, size, "%m/%d/%Y", &tm_info);
}

void get_current_date(int *day, int *month, int *year) {
    const time_t now = time(NULL);

    struct tm tm_info = *localtime(&now);
    *day = tm_info.tm_mday;
    *month = tm_info.tm_mon + 1;
    *year = tm_info.tm_year + 1900;
}