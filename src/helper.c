#include "helper.h"

void get_currente_date(char *buffer, size_t size) {
    const time_t now = time(NULL);

    struct tm tm_info = *localtime(&now);
    strftime(buffer, size, "%m-%d-%Y", &tm_info);
}