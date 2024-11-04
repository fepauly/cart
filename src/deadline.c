#include "deadline.h"
#include "pretty.h"
#include "commands.h"
#include "cart.h"
#include "helper.h"
#include <string.h>

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

    char deadline[11];
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
    if (cart_handler_open(&cartHandler, filename, "r+") != 0) {
        print_colored(ERROR_COLOR, "Failed to read %s!", filename);
        return -1;
    }

    if (cart_handler_set_meta_entry(&cartHandler, "deadline", deadline) != 0) {
        print_colored(ERROR_COLOR, "Failed to set deadline entry!");
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
    print_colored(GREEN_COLOR, "Updated project deadline to '%s' successfully!", deadline);
    return 0;
}

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
    if (cart_handler_open(&cartHandler, filename, "r+") != 0) {
        print_colored(ERROR_COLOR, "Failed to read %s!", filename);
        return -1;
    }

    char value[11] = {0};

    if (cart_handler_get_meta_entry(&cartHandler, "deadline", value, sizeof(value)) != 0) {
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
    print_colored(GREEN_COLOR, "%s: %s", "deadline", value);
    return 0;
}

int cmd_deadline_check(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart deadline check\n");
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
    if (cart_handler_open(&cartHandler, filename, "r+") != 0) {
        print_colored(ERROR_COLOR, "Failed to read %s!", filename);
        return -1;
    }

    char value[11] = {0};
    if (cart_handler_get_meta_entry(&cartHandler, "deadline", value, sizeof(value)) != 0) {
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
    return 0;
}