#include "meta.h"
#include "pretty.h"
#include "commands.h"
#include "cart.h"
#include <string.h>

// Function to handle the 'meta' command
int cmd_meta(int argc, char *argv[]) {
    if (argc < 2) {
        print_colored(ERROR_COLOR, "NOPE. There is a command missing my friend. Usage: cart meta <subcommand> [options]");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        printf("Usage: cart meta <subcommand> [options]\n\nSubcommands:\n  set: set a metadata entry.\n  get: Get a metadata entry.\n"
                "  list: List all metadata entries.\n");
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

// Function to handle the 'meta set' command
int cmd_meta_set(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart meta set <entry> <value>\n");
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "created") == 0) {
        printf("The entry 'created' cannot be updated. Sorry.\n");
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "deadline") == 0) {
        printf("To set the deadline use: cart deadline set -d <day> -m <month> -y <year>\n");
        return 0;
    }
    if (argc == 2) {
        print_colored(ERROR_COLOR, "NOPE. There are values missing my friend. Usage: cart meta set <entry> <value>");
        return -1;
    }

    char filename[MAX_STR_LEN] = {0};
    if (find_cart_file(filename, sizeof(filename)) == 0) {
        print_colored(BLUE_COLOR, "Found project: %s\n", filename);
    } else {
        print_colored(ERROR_COLOR, "Couldn't find a CART project in current directory my friend!");
        return -1;
    }

    // Initialize cart and cartHandler structures
    Cart cart = {0};
    CartHandler cartHandler = {0};
    
    if (cart_handler_open(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to read %s!", filename);
        return -1;
    }

    if(cart_handler_read_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    const char *entry = argv[1];
    const char *new_value = argv[2];

    if (cart_handler_set_meta_entry(&cart, entry, new_value) != 0) {
        print_colored(ERROR_COLOR, "Failed to set metadata entry!");
        free_cart(&cart);
        cart_handler_close(&cartHandler);
        return -1;
    }

    if(cart_handler_write_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        free_cart(&cart);
        cart_handler_close(&cartHandler);
        return -1;
    }

    if (cart_handler_save(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to set metadata entry to file!");
        free_cart(&cart);
        cart_handler_close(&cartHandler);
        return -1;
    }
    
    free_cart(&cart);
    cart_handler_close(&cartHandler);
    print_colored(GREEN_COLOR, "Updated metadata entry %s to '%s' successfully!", entry, new_value);
    return 0;
}

// Function to handle the 'meta get' command
int cmd_meta_get(int argc, char *argv[]) {
    if (argc == 1) {
        print_colored(ERROR_COLOR, "NOPE. There are values missing my friend. Usage: cart meta get <entry>");
        return -1;
    }
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart meta get <entry>\n");
        return 0;
    }
    char filename[MAX_STR_LEN] = {0};
    if (find_cart_file(filename, sizeof(filename)) == 0) {
        print_colored(BLUE_COLOR, "Found project: %s\n", filename);
    } else {
        print_colored(ERROR_COLOR, "Couldn't find a CART project in current directory my friend!");
        return -1;
    }
    
    // We must initialize cart before using it
    Cart cart = {0};
    
    // Open project file
    CartHandler cartHandler = {0}; // Initialize cartHandler
    
    if (cart_handler_open(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to read %s!", filename);
        return -1;
    }
    
    const char *entry = argv[1];
    char value[MAX_STR_LEN] = {0};
    
    if(cart_handler_read_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }
    
    if (cart_handler_get_meta_entry(&cart, entry, value) != 0) {
        print_colored(ERROR_COLOR, "Entry not found or empty!");
        free_cart(&cart);
        cart_handler_close(&cartHandler);
        return -1;
    }
    
    free_cart(&cart);
    cart_handler_close(&cartHandler);
    print_colored(GREEN_COLOR, "%s: %s", entry, value);
    return 0;
}

// Function to handle the 'meta list' command
int cmd_meta_list(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart meta list\n");
        return 0;
    }

    char filename[MAX_STR_LEN] = {0};
    if (find_cart_file(filename, sizeof(filename)) == 0) {
        print_colored(BLUE_COLOR, "Found project: %s\n", filename);
    } else {
        print_colored(ERROR_COLOR, "Couldn't find a CART project in current directory my friend!");
        return -1;
    }

    // Initialize cart and cartHandler structures
    Cart cart = {0};
    CartHandler cartHandler = {0};
    
    if (cart_handler_open(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to read %s!", filename);
        return -1;
    }

    if(cart_handler_read_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    if (cart_handler_list_meta(&cart) != 0) {
        print_colored(ERROR_COLOR, "Error printing metadata!");
        free_cart(&cart);
        cart_handler_close(&cartHandler);
        return -1;
    }
    
    xmlCleanupParser();
    free_cart(&cart);
    cart_handler_close(&cartHandler);
    return 0;
}