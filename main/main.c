#include "cart.h"
#include "pretty.h"
#include "commands.h"
#include "string.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_colored(ERROR_COLOR, "NOPE. There is a command missing my friend. Usage: cart <command> [options]");
        return 1;
    }

    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(commands[i].name, argv[1]) == 0) {
            return commands[i].function(argc - 1, argv + 1);
        }
    }

    print_colored(ERROR_COLOR, "NOPE. I don't know that commands my friend!");
    return 1;
}