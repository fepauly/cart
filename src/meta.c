#include "meta.h"
#include "pretty.h"
#include "commands.h"
#include "cart.h"
#include <string.h>

int cmd_meta(int argc, char *argv[]) {
    if (argc < 2) {
        print_colored(ERROR_COLOR, "NOPE. There is a command missing my friend. Usage: cart meta <subcommand> [options]");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        printf("Usage: cart meta <subcommand> [options]\n\nSubcommands:\n  update: Update a metadata entry.\n  get: Get a metadata entry.\n");
        return 0;
    }

    for (int i = 0; meta_commands[i].name != NULL; i++) {
        if (strcmp(meta_commands[i].name, argv[1]) == 0) {
            return meta_commands[i].function(argc - 1, argv + 1);
        }
    }

    print_colored(ERROR_COLOR, "NOPE. I don't know that commands my friend!\nTry 'cart meta help'");
    return -1;
}

int cmd_meta_update(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart meta update <entry> <value>\n");
        return 0;
    }
    if (argc == 2) {
        print_colored(ERROR_COLOR, "NOPE. There are values missing my friend. Usage: cart meta update <entry> <value>");
        return -1;
    }


    char filename[260] = {0};
    if (find_cart_file(filename, sizeof(filename)) == 0) {
        print_colored(GREEN_COLOR, "Found project: %s", filename);
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

    const char *entry = argv[1];
    const char *new_value = argv[2];

    if (cart_handler_update_meta_entry(&cartHandler, entry, new_value) != 0) {
        print_colored(ERROR_COLOR, "Failed to set metadata entry!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    if (cart_handler_save(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to update metadata entry to file!");
        cart_handler_close(&cartHandler);
        return -1;
    }
    cart_handler_close(&cartHandler);
    print_colored(GREEN_COLOR, "Updated metadata entry %s to '%s' successfully!", entry, new_value);
    return 0;
}

int cmd_meta_get(int argc, char *argv[]) {
    if (argc == 1) {
        print_colored(ERROR_COLOR, "NOPE. There are values missing my friend. Usage: cart meta get <entry>");
        return -1;
    }
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart meta get <entry>\n");
        return 0;
    }

    char filename[260] = {0};
    if (find_cart_file(filename, sizeof(filename)) == 0) {
        print_colored(GREEN_COLOR, "Found project: %s", filename);
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

    const char *entry = argv[1];
    char value[512] = {0};

    if (cart_handler_get_meta_entry(&cartHandler, entry, value) != 0) {
        print_colored(ERROR_COLOR, "Entry not found or empty!");
        cart_handler_close(&cartHandler);
        return -1;
    }
    cart_handler_close(&cartHandler);
    print_colored(GREEN_COLOR, "%s: %s", entry, value);
    return 0;
}