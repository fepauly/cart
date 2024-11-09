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
    if (argc < 2) {
        print_colored(ERROR_COLOR, "NOPE. There are values missing my friend. Usage: cart feature add name -d <description> -a <alias> -t <tag>");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        printf("Usage: cart feature add name -d <description> -a <alias> -t <tag>\n");
        return 0;
    }
    /*
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

    Feature feature = {0};
    strncopy(feature.name, argv[1], sizeof(feature.name) - 1);
    feature.name[sizeof(feature.name) - 1] = '\0';

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            strncopy(feature.description, argv[++i], sizeof(feature.description) - 1);
            feature.description[sizeof(feature.description) - 1] = '\0';
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            strncopy(feature.alias, argv[++i], sizeof(feature.alias) - 1);
            feature.alias[sizeof(feature.alias) - 1] = '\0';
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            strncopy(tag.tag, argv[++i], sizeof(tag.tag) - 1);
            tag.tag[sizeof(tag.tag) - 1] = '\0';
        }
    }
    */
   return 0;
}