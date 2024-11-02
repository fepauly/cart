#include "init.h"
#include "cart.h"
#include "pretty.h"
#include "helper.h"
#include "string.h"

int cmd_init(int argc, char *argv[]) {
    if (argc == 1) {
        print_colored(ERROR_COLOR, "NOPE. Please provide a project name.\nFor more help try: cart init help");
        return -1;
    }

    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("Usage: cart init <name> [options]\n\nOptions:\n  --author, -a: Author of the project.\n  "
               "--description, -d: Short description of the project.\n  --version, -v: Version of the project.\n");
        return 0;
    }

    char filename[260] = {0};
    if (find_cart_file(filename, sizeof(filename)) == 0) {
        print_colored(ERROR_COLOR, "Found an existing CART File: %s", filename);
        return -1;
    }

    if (strlen(argv[1]) >= 256) {
        print_colored(ERROR_COLOR, "NOPE. The project name is too long my friend!");
        return -1;
    }

    Project project = {0};
    strcpy(project.name, argv[1]);

    char date[11];
    get_currente_date(date, sizeof(date));
    strcpy(project.created, date);

    for (int i = 2; i < argc; i++) {
        if ((strcmp(argv[i], "--author") == 0 || strcmp(argv[i], "-a") == 0) && i + 1 < argc) {
            strncpy(project.author, argv[i+1], sizeof(project.author) - 1);
        }
        else if ((strcmp(argv[i], "--version") == 0|| strcmp(argv[i], "-v") == 0) && i + 1 < argc) {
            strncpy(project.version, argv[i+1], sizeof(project.version) - 1);
        }
        else if ((strcmp(argv[i], "--description") == 0 || strcmp(argv[i], "-d") == 0) && i + 1 < argc) {
            strncpy(project.description, argv[i+1], sizeof(project.description) - 1);
        }
    }

    char project_file[260] = {0};
    snprintf(project_file, sizeof(project_file), "%s.cart", project.name);

    CartHandler cartHandler;
    if (cart_handler_open(&cartHandler, project_file, "w") != 0) {
        print_colored(ERROR_COLOR, "Failed to create %s!", project_file);
        return -1;
    }


    if (cart_handler_init_project_section(&cartHandler, &project) != 0) {
        print_colored(ERROR_COLOR, "Error while creating CART file!");
    }
    cart_handler_close(&cartHandler);
    print_colored(GREEN_COLOR, "Initialized project %s successfully!", project.name);
    return 0;
}