#include "pretty.h"
#include "commands.h"

// Function to handle the 'help' command
int cmd_help(int argc, char *argv[]) {
    print_colored(GREEN_COLOR, "This is the help message for CART project management tool!");
    printf("Usage: cart <command> [options]\n\n");
    printf("Commands:\n"); 
    for (int i = 0; commands[i].name != NULL; i++) {
        printf("  %s: %s\n", commands[i].name, commands[i].help);
    }
    return 0; 
}