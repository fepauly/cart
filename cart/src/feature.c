#include "feature.h"
#include "pretty.h"
#include "commands.h"
#include "cart.h"
#include "id.h"

// Function to handle the 'feature' command
int cmd_feature(int argc, char *argv[]) {
    if (argc < 2) {
        print_colored(ERROR_COLOR, "NOPE. There is a command missing my friend. Usage: cart feature <subcommand> [options]");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        printf("Usage: cart feature <subcommand> [options]\n\nSubcommands:\n  add: Add a feature to the project.\n  get: Get a project feature.\n  remove: Remove a project feature.\n  update: Update a project feature.\n");
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

// Function to handle the 'feature add' command
int cmd_feature_add(int argc, char *argv[]) {
    if (argc < 2) {
        print_colored(ERROR_COLOR, "NOPE. There are values missing my friend. Usage: cart feature add name -d <description> -a <alias> -t <tag>");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        printf("Usage: cart feature add name -d <description> -a <alias> -t <tag>\n");
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
    
    if (cart_handler_read_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }
    
    // Check if we've reached the maximum number of features
    if (cart.num_features >= MAX_FEATURES) {
        print_colored(ERROR_COLOR, "Maximum number of features (%d) reached!", MAX_FEATURES);
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }

    // Initialize a new feature
    Feature *feature = (Feature *)malloc(sizeof(Feature));
    if (!feature) {
        print_colored(ERROR_COLOR, "Memory allocation failed!");
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }
    memset(feature, 0, sizeof(Feature));
    
    // Generate a new ID for the feature
    char next_id[MAX_ID_LEN];
    if (generate_unique_id(next_id, sizeof(next_id)) != 0) {
        print_colored(ERROR_COLOR, "Failed to generate ID for feature!");
        free(feature);
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }
    strncpy(feature->id, next_id, sizeof(feature->id) - 1);
    feature->id[sizeof(feature->id) - 1] = '\0';
    
    // Set the name (required)
    strncpy(feature->name, argv[1], sizeof(feature->name) - 1);
    feature->name[sizeof(feature->name) - 1] = '\0';
    
    // Default values
    feature->status = STATUS_OPEN;
    feature->priority = PRIORITY_MEDIUM;
    
    // Parse optional arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            strncpy(feature->description, argv[++i], sizeof(feature->description) - 1);
            feature->description[sizeof(feature->description) - 1] = '\0';
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            strncpy(feature->alias, argv[++i], sizeof(feature->alias) - 1);
            feature->alias[sizeof(feature->alias) - 1] = '\0';
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc && feature->num_tags < MAX_TAGS) {
            strncpy(feature->tags[feature->num_tags], argv[++i], sizeof(feature->tags[feature->num_tags]) - 1);
            feature->tags[feature->num_tags][sizeof(feature->tags[feature->num_tags]) - 1] = '\0';
            feature->num_tags++;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            char *priority_str = argv[++i];
            if (strcasecmp(priority_str, "low") == 0) {
                feature->priority = PRIORITY_LOW;
            } else if (strcasecmp(priority_str, "medium") == 0) {
                feature->priority = PRIORITY_MEDIUM;
            } else if (strcasecmp(priority_str, "high") == 0) {
                feature->priority = PRIORITY_HIGH;
            } else if (strcasecmp(priority_str, "critical") == 0) {
                feature->priority = PRIORITY_CRITICAL;
            } else {
                print_colored(ERROR_COLOR, "Invalid priority: %s. Using default (MEDIUM).", priority_str);
            }
        }
    }
    
    // Add the feature to the cart
    cart.features[cart.num_features++] = feature;
    
    // Save changes to the file
    if (cart_handler_write_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error writing XML!");
        free(feature);
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }
    
    if (cart_handler_save(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to save feature to file!");
        free(feature);
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }
    
    xmlCleanupParser();
    cart_handler_close(&cartHandler);
    free_cart(&cart);
    
    print_colored(GREEN_COLOR, "Added feature '%s' with ID '%s' successfully!", feature->name, feature->id);
    return 0;
}

// Function to handle the 'feature get' command
int cmd_feature_get(int argc, char *argv[]) {
    if (argc < 2) {
        print_colored(ERROR_COLOR, "NOPE. Missing feature ID. Usage: cart feature get <id>");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        printf("Usage: cart feature get <id>\n");
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
    
    if (cart_handler_read_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    // Find the feature with the given ID
    Feature *found_feature = NULL;
    for (int i = 0; i < cart.num_features; i++) {
        if (strcmp(cart.features[i]->id, argv[1]) == 0) {
            found_feature = cart.features[i];
            break;
        }
    }

    if (!found_feature) {
        print_colored(ERROR_COLOR, "Feature with ID '%s' not found!", argv[1]);
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }

    // Display feature details
    printf("\n");
    print_colored(GREEN_COLOR, "Feature #%s: %s", found_feature->id, found_feature->name);
    printf("\n");
    
    if (strlen(found_feature->alias) > 0) {
        printf("Alias: %s\n", found_feature->alias);
    }
    
    printf("Description: %s\n", found_feature->description);
    printf("Status: %s\n", status_to_string(found_feature->status));
    printf("Priority: %s\n", priority_to_string(found_feature->priority));
    
    if (found_feature->num_tags > 0) {
        printf("Tags: ");
        for (int i = 0; i < found_feature->num_tags; i++) {
            printf("%s%s", found_feature->tags[i], (i < found_feature->num_tags - 1) ? ", " : "");
        }
        printf("\n");
    }

    cart_handler_close(&cartHandler);
    free_cart(&cart);
    
    return 0;
}

// Function to handle the 'feature remove' command
int cmd_feature_remove(int argc, char *argv[]) {
    if (argc < 2) {
        print_colored(ERROR_COLOR, "NOPE. Missing feature ID. Usage: cart feature remove <id>");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        printf("Usage: cart feature remove <id>\n");
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
    
    if (cart_handler_read_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    // Find the feature with the given ID
    int found_index = -1;
    for (int i = 0; i < cart.num_features; i++) {
        if (strcmp(cart.features[i]->id, argv[1]) == 0) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        print_colored(ERROR_COLOR, "Feature with ID '%s' not found!", argv[1]);
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }

    // Store the name for a confirmation message
    char feature_name[MAX_STR_LEN];
    strncpy(feature_name, cart.features[found_index]->name, sizeof(feature_name) - 1);
    feature_name[sizeof(feature_name) - 1] = '\0';

    // Release the ID to be reused later
    release_id(cart.features[found_index]->id);

    // Free the memory for the feature
    free(cart.features[found_index]);

    // Shift all features after the removed one
    for (int i = found_index; i < cart.num_features - 1; i++) {
        cart.features[i] = cart.features[i + 1];
    }
    
    // Update the feature count and set the last pointer to NULL
    cart.features[--cart.num_features] = NULL;

    // Save changes to the file
    if (cart_handler_write_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error writing XML!");
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }
    
    if (cart_handler_save(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to save changes to file!");
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }
    
    xmlCleanupParser();
    cart_handler_close(&cartHandler);
    free_cart(&cart);
    
    print_colored(GREEN_COLOR, "Removed feature '%s' with ID '%s' successfully!", feature_name, argv[1]);
    return 0;
}

// Function to handle the 'feature update' command
int cmd_feature_update(int argc, char *argv[]) {
    if (argc < 3) {
        print_colored(ERROR_COLOR, "NOPE. There are values missing my friend. Usage: cart feature update <id> [-n <name>] [-d <description>] [-a <alias>] [-t <tag>] [-s <status>] [-p <priority>]");
        return -1;
    }

    if (strcmp(argv[1], "help") == 0) {
        printf("Usage: cart feature update <id> [-n <name>] [-d <description>] [-a <alias>] [-t <tag>] [-s <status>] [-p <priority>]\n");
        printf("Status options: open, in_progress, done\n");
        printf("Priority options: low, medium, high, critical\n");
        return 0;
    }

    char *feature_id = argv[1];
    
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
    
    if (cart_handler_read_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error interpreting XML!");
        cart_handler_close(&cartHandler);
        return -1;
    }

    // Find the feature with the given ID
    Feature *found_feature = NULL;
    for (int i = 0; i < cart.num_features; i++) {
        if (strcmp(cart.features[i]->id, feature_id) == 0) {
            found_feature = cart.features[i];
            break;
        }
    }

    if (!found_feature) {
        print_colored(ERROR_COLOR, "Feature with ID '%s' not found!", feature_id);
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }

    // Keep track of whether we made any changes
    int changes_made = 0;

    // Parse update parameters
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            strncpy(found_feature->name, argv[++i], sizeof(found_feature->name) - 1);
            found_feature->name[sizeof(found_feature->name) - 1] = '\0';
            changes_made = 1;
        } else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            strncpy(found_feature->description, argv[++i], sizeof(found_feature->description) - 1);
            found_feature->description[sizeof(found_feature->description) - 1] = '\0';
            changes_made = 1;
        } else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            strncpy(found_feature->alias, argv[++i], sizeof(found_feature->alias) - 1);
            found_feature->alias[sizeof(found_feature->alias) - 1] = '\0';
            changes_made = 1;
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            char *status_str = argv[++i];
            Status new_status = STATUS_UNKNOWN;
            
            if (strcasecmp(status_str, "open") == 0) {
                new_status = STATUS_OPEN;
            } else if (strcasecmp(status_str, "in_progress") == 0) {
                new_status = STATUS_IN_PROGRESS;
            } else if (strcasecmp(status_str, "done") == 0) {
                new_status = STATUS_DONE;
            } else {
                print_colored(ERROR_COLOR, "Invalid status: %s. Valid options are: open, in_progress, done", status_str);
                continue;
            }
            
            found_feature->status = new_status;
            changes_made = 1;
        } else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            char *priority_str = argv[++i];
            Priority new_priority = PRIORITY_UNKNOWN;
            
            if (strcasecmp(priority_str, "low") == 0) {
                new_priority = PRIORITY_LOW;
            } else if (strcasecmp(priority_str, "medium") == 0) {
                new_priority = PRIORITY_MEDIUM;
            } else if (strcasecmp(priority_str, "high") == 0) {
                new_priority = PRIORITY_HIGH;
            } else if (strcasecmp(priority_str, "critical") == 0) {
                new_priority = PRIORITY_CRITICAL;
            } else {
                print_colored(ERROR_COLOR, "Invalid priority: %s. Valid options are: low, medium, high, critical", priority_str);
                continue;
            }
            
            found_feature->priority = new_priority;
            changes_made = 1;
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            if (found_feature->num_tags >= MAX_TAGS) {
                print_colored(ERROR_COLOR, "Maximum number of tags (%d) reached!", MAX_TAGS);
                continue;
            }
            
            strncpy(found_feature->tags[found_feature->num_tags], argv[++i], sizeof(found_feature->tags[found_feature->num_tags]) - 1);
            found_feature->tags[found_feature->num_tags][sizeof(found_feature->tags[found_feature->num_tags]) - 1] = '\0';
            found_feature->num_tags++;
            changes_made = 1;
        }
    }

    if (!changes_made) {
        print_colored(ERROR_COLOR, "No changes were made to feature '%s'", found_feature->name);
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }

    // Save changes to the file
    if (cart_handler_write_project(&cartHandler, &cart) != 0) {
        print_colored(ERROR_COLOR, "Error writing XML!");
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }
    
    if (cart_handler_save(&cartHandler, filename) != 0) {
        print_colored(ERROR_COLOR, "Failed to save changes to file!");
        cart_handler_close(&cartHandler);
        free_cart(&cart);
        return -1;
    }
    
    xmlCleanupParser();
    cart_handler_close(&cartHandler);
    free_cart(&cart);
    
    print_colored(GREEN_COLOR, "Updated feature '%s' with ID '%s' successfully!", found_feature->name, feature_id);
    return 0;
}