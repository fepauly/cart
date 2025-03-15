#include "deadline.h"
#include "pretty.h"
#include "commands.h"
#include "cart.h"
#include "helper.h"
#include <string.h>
#include <time.h>

// Function to handle the 'deadline' command
int cmd_deadline(int argc, char *argv[]) {
    if (argc < 2) {
        print_colored(ERROR_COLOR, "NOPE. There is a command missing my friend. Usage: cart deadline <subcommand> [options]");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        printf("Usage: cart deadline <subcommand> [options]\n\nSubcommands:\n  set: set the project deadline.\n  get: Get the project deadline.\n");
        return 0;
    }

    for (int i = 0; deadline_commands[i].name != NULL; i++) {
        if (strcmp(deadline_commands[i].name, argv[1]) == 0) {
            return deadline_commands[i].function(argc - 1, argv + 1);
        }
    }

    print_colored(ERROR_COLOR, "NOPE. I don't know that commands my friend!\nTry 'cart deadline help'");
    return -1;
}

// Function to handle the 'deadline set' command
int cmd_deadline_set(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart deadline set -d <day> -m <month> -y <year>\n");
        return 0;
    }

    if (argc < 6) {
        print_colored(ERROR_COLOR, "NOPE. There are values missing my friend. Usage: cart deadline set -d <day> -m <month> -y <year>");
        return -1;
    }

    int day = -1, month = -1, year = -1;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            day = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            month = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-y") == 0 && i + 1 < argc) {
            year = atoi(argv[++i]);
        }
    }

    if (day == -1 || month == -1 || year == -1) {
        print_colored(ERROR_COLOR, "Usage: cart deadline set -d <day> -m <month> -y <year>\n");
        return -1;
    }

    struct tm date = {0};
    int current_day, current_month, current_year;
    get_current_date(&date);
    
    current_day = date.tm_mday;
    current_month = date.tm_mon + 1;
    current_year = date.tm_year + 1900;

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < current_year ||
        (year == current_year && month < current_month) ||
        (year == current_year && month == current_month && day < current_day)) {
        print_colored(ERROR_COLOR, "Invalid date my friend! Check your ranges and remember: Deadline cannot be set before today's date.\n");
        return -1;
    }

    char deadline[MAX_DATE_LEN];
    snprintf(deadline, sizeof(deadline), "%02d/%02d/%04d", month, day, year);

    char filename[260] = {0};
    if (find_cart_file(filename, sizeof(filename)) == 0) {
        print_colored(BLUE_COLOR, "Found project: %s\n", filename);
    } else {
        print_colored(ERROR_COLOR, "Couldn't find a CART project in current directory my friend!");
        return -1;
    }

    // Open project file
    CartHandler cartHandler;
    Cart cart = {0};
    if (cart_handler_open(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to read %s!", filename);
        return -1;
    }

    if(cart_handler_read_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    if (cart_handler_set_meta_entry(&cart, "deadline", deadline) != 0) {
        print_colored(ERROR_COLOR, "Failed to set deadline entry!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    if(cart_handler_write_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    if (cart_handler_save(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to save deadline entry to file!");
        cart_handler_close(&cartHandler);
        return -1;
    }
    xmlCleanupParser();
    cart_handler_close(&cartHandler);
    free_cart(&cart);
    print_colored(GREEN_COLOR, "Updated project deadline to '%s' successfully!", deadline);
    return 0;
}

// Function to handle the 'deadline get' command
int cmd_deadline_get(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart deadline get\n");
        return 0;
    }

    char filename[260] = {0};
    if (find_cart_file(filename, sizeof(filename)) == 0) {
        print_colored(BLUE_COLOR, "Found project: %s\n", filename);
    } else {
        print_colored(ERROR_COLOR, "Couldn't find a CART project in current directory my friend!");
        return -1;
    }

    // Open project file
    CartHandler cartHandler;
    Cart cart = {0};
    if (cart_handler_open(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to read %s!", filename);
        return -1;
    }

    if(cart_handler_read_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    char value[MAX_DATE_LEN] = {0};

    if (cart_handler_get_meta_entry(&cart, "deadline", value) != 0) {
        print_colored(ERROR_COLOR, "Entry not found or empty!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    xmlCleanupParser();
    cart_handler_close(&cartHandler);
    if (strlen(value) == 0) {
        print_colored(ERROR_COLOR, "No deadline set for the project! Use 'cart deadline set -d <day> -m <month> -y <year>'");
        return -1;
    }
    free_cart(&cart);
    print_colored(GREEN_COLOR, "%s: %s", "deadline", value);
    return 0;
}

// Function to handle the 'deadline check' command
int cmd_deadline_check(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart deadline check\n");
        return 0;
    }

    char filename[MAX_STR_LEN] = {0};
    if (find_cart_file(filename, sizeof(filename)) == 0) {
        print_colored(BLUE_COLOR, "Found project: %s\n", filename);
    } else {
        print_colored(ERROR_COLOR, "Couldn't find a CART project in current directory my friend!");
        return -1;
    }

    // Open project file
    CartHandler cartHandler;
    Cart cart = {0};
    if (cart_handler_open(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to read %s!", filename);
        return -1;
    }

    if(cart_handler_read_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    char value[MAX_DATE_LEN] = {0};
    if (cart_handler_get_meta_entry(&cart, "deadline", value) != 0) {
        print_colored(ERROR_COLOR, "Entry not found or empty!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    if (strlen(value) == 0) {
        print_colored(ERROR_COLOR, "No deadline set for the project! Use 'cart deadline set -d <day> -m <month> -y <year>'");
        xmlCleanupParser();
        cart_handler_close(&cartHandler);
        return -1;
    }
    int days_between;
    struct tm datenow = {0};
    struct tm datedeadline = {0};
    if (get_current_date(&datenow) != 0) {
        print_colored(ERROR_COLOR, "Error while getting current date!");
        xmlCleanupParser();
        cart_handler_close(&cartHandler);
        return -1;
    }
    if (convert_str_to_date(value, &datedeadline) != 0) {
        print_colored(ERROR_COLOR, "Error converting string to date!");
        xmlCleanupParser();
        cart_handler_close(&cartHandler);
        return -1;
    }

    days_between = days_between_dates(&datenow, &datedeadline);
    if (days_between < 0) {
        print_colored(ERROR_COLOR, "OH OH. Deadline already met my friend!\n\U000023F1 : %s", value);
    } else if (days_between == 0) {
        print_colored(ERROR_COLOR, "WOW. Less than a day left my friend! Hurry up!\n\U000023F1 : %s", value);
    } else {
        print_colored(GREEN_COLOR, "You have %d day(s) left my friend!\n\U000023F1 : %s", days_between, value);
    }

    xmlCleanupParser();
    cart_handler_close(&cartHandler);
    free_cart(&cart);
    return 0;
}

// Helper function to get current date as string in MM/DD/YYYY format
int get_current_date_str(char *date_str, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    if (date_str == NULL || size < 11) { // Need at least 11 chars for MM/DD/YYYY\0
        return -1;
    }
    
    snprintf(date_str, size, "%02d/%02d/%04d", 
             t->tm_mon + 1,    // tm_mon is 0-based
             t->tm_mday,
             t->tm_year + 1900); // tm_year is years since 1900
    
    return 0;
}

// Helper function to get current date in tm structure
int get_current_date(struct tm *date) {
    if (date == NULL) {
        return -1;
    }
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    *date = *t;
    return 0;
}

// Helper function to convert string date to tm structure
int convert_str_to_date(const char *date_str, struct tm *date) {
    if (date_str == NULL || date == NULL) {
        return -1;
    }
    
    int month, day, year;
    if (sscanf(date_str, "%d/%d/%d", &month, &day, &year) != 3) {
        return -1;
    }
    
    memset(date, 0, sizeof(struct tm));
    date->tm_year = year - 1900;  // Convert to years since 1900
    date->tm_mon = month - 1;     // Convert to 0-based month
    date->tm_mday = day;
    
    return 0;
}

// Helper function to calculate days between two dates
int days_between_dates(struct tm *date1, struct tm *date2) {
    if (date1 == NULL || date2 == NULL) {
        return -1;
    }
    
    // Convert to time_t (seconds since epoch)
    time_t time1 = mktime(date1);
    time_t time2 = mktime(date2);
    
    if (time1 == -1 || time2 == -1) {
        return -1;
    }
    
    // Calculate difference in days
    const int seconds_per_day = 60 * 60 * 24;
    return (int)((time2 - time1) / seconds_per_day);
}