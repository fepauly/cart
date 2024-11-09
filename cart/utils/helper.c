#include "helper.h"
#include "pretty.h"
#include "string.h"


int get_current_date_str(char *buffer, size_t size) {
    const time_t now = time(NULL);

    struct tm tm_info = *localtime(&now);
    strftime(buffer, size, "%m/%d/%Y", &tm_info);
    return 0;
}

int get_current_date(struct tm *date){
    const time_t now = time(NULL);

    *date = *localtime(&now);
    return 0;
}

int convert_str_to_date(const char* datestr, struct tm *date) {
    if (datestr == NULL || strlen(datestr) == 0) {
        print_colored(ERROR_COLOR, "Date is empty!");
        return -1;
    }

    int day, month, year;
    if (sscanf(datestr, "%d/%d/%d", &month, &day, &year) == 3) {
        date->tm_mday = day;
        date->tm_mon = month - 1;
        date->tm_year = year - 1900;
        return 0;
    } else {
        print_colored(ERROR_COLOR, "Error parsing date!");
        return -1;
    }
}

int days_between_dates(struct tm *date1, struct tm *date2) {
    time_t time1 = mktime(date1);
    time_t time2 = mktime(date2);

    double seconds_diff = difftime(time2, time1);
    int days_diff = seconds_diff / (60 * 60 * 24);
    return days_diff;
}

