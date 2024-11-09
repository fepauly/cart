#include "feature.h"
#include "pretty.h"
#include "commands.h"
#include "cart.h"

int cmd_feature(int argc, char *argv[]) {
    if (argc < 2) {
        print_colored(ERROR_COLOR, "NOPE. There is a command missing my friend. Usage: cart feature <subcommand> [options]");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        printf("Usage: cart feature <subcommand> [options]\n\nSubcommands:\n  add: Add a feature to the project.\n  get: Get a project feature.\n");
        return 0;
    }

    for (int i = 0; feature_commands[i].name != NULL; i++) {
        if (strcmp(feature_commands[i].name, argv[1]) == 0) {
            return feature_commands[i].function(argc - 1, argv + 1);
        }
    }

    print_colored(ERROR_COLOR, "NOPE. I don't know that commands my friend!\nTry 'cart feature help'");
    return -1;
}

int cmd_feature_add(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart feature add name -d <description> -a <alias> -t <tag>\n");
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
}