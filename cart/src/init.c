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

    char filename[MAX_STR_LEN] = {0};
    if (find_cart_file(filename, sizeof(filename)) == 0) {
        print_colored(ERROR_COLOR, "Found an existing CART File: %s", filename);
        return -1;
    }

    if (strlen(argv[1]) >= MAX_STR_LEN) {
        print_colored(ERROR_COLOR, "NOPE. The project name is too long my friend!");
        return -1;
    }

    // Create metadata name from inputs
    Cart cart = {0};
    cart.num_features = 0;
    Metadata metadata = {0};
    strncpy(metadata.name, argv[1], sizeof(metadata.name) - 1);
    metadata.name[sizeof(metadata.name) - 1] = '\0'; 

    // Get current date
    char date[MAX_DATE_LEN];
    get_current_date_str(date, sizeof(date));
    strcpy(metadata.created, date);

    // Set metadata
    for (int i = 2; i < argc; i++) {
        if ((strcmp(argv[i], "--author") == 0 || strcmp(argv[i], "-a") == 0) && i + 1 < argc) {
            strncpy(metadata.author, argv[++i], sizeof(metadata.author) - 1);
            metadata.author[sizeof(metadata.author) - 1] = '\0';
        }
        else if ((strcmp(argv[i], "--version") == 0|| strcmp(argv[i], "-v") == 0) && i + 1 < argc) {
            strncpy(metadata.version, argv[++i], sizeof(metadata.version) - 1);
            metadata.version[sizeof(metadata.version) - 1] = '\0';
        }
        else if ((strcmp(argv[i], "--description") == 0 || strcmp(argv[i], "-d") == 0) && i + 1 < argc) {
            strncpy(metadata.description, argv[++i], sizeof(metadata.description) - 1);
            metadata.description[sizeof(metadata.description) - 1] = '\0';
        }
    }
    cart.metadata = &metadata;

    // Set project file
    char project_file[MAX_STR_LEN + 5] = {0};
    snprintf(project_file, sizeof(project_file), "%s.cart", metadata.name);

    CartHandler cartHandler = {0};    

    if (cart_handler_write_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error writing CART to XML representation!");
        return -1;
    }  

    if (cart_handler_save(&cartHandler, project_file) != 0) {
        cart_handler_close(&cartHandler);
        print_colored(GREEN_COLOR, "Failed to initialize project %s!", metadata.name);
        return -1;
    }
    cart_handler_close(&cartHandler);
    print_colored(GREEN_COLOR, "Initialized project %s successfully!", metadata.name);
    return 0;
}