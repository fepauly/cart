#include "cart.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "pretty.h"
#include "helper.h"
#include "id.h"

// Initialize the global function pointer
find_cart_file_fn find_cart_file_func = NULL;

// Function to open a CART file and parse its XML content
int cart_handler_open(CartHandler *handler, const char *filename) {
    handler->doc = xmlParseFile(filename);
    if(handler->doc == NULL) {
        print_colored(ERROR_COLOR, "Failed to parse %s.\n", filename);
        return -1;
    }

    xmlNodePtr root_node = xmlDocGetRootElement(handler->doc);
    if (root_node == NULL) {
        print_colored(ERROR_COLOR, "The project is empty my friend! (file: %s)", filename);
        xmlFreeDoc(handler->doc);
        handler->doc = NULL;
        return -1;
    } 

    return 0;
}

// Function to close a CART file and free its resources
int cart_handler_close(CartHandler *handler) {
    if (handler == NULL) {
        print_colored(ERROR_COLOR, "Invalid handler!");
        return -1;
    }
    
    if (handler->doc) {
        xmlFreeDoc(handler->doc);
        handler->doc = NULL;
        xmlCleanupParser();  
        return 0;
    }
    print_colored(ERROR_COLOR, "File already closed!");
    return -1;
}

// Function to save the CART file to disk
int cart_handler_save(CartHandler *handler, const char *filename) {
    if (handler == NULL || handler->doc == NULL) {
        print_colored(ERROR_COLOR, "File handler or document is NULL!");
        return -1;
    }

    if (xmlSaveFormatFileEnc(filename, handler->doc, "UTF-8", 1) == -1) {
        print_colored(ERROR_COLOR, "Failed to save CART file: %s", filename);
        return -1;
    }

    return 0;
} 

// Function to set a metadata entry in the CART structure
int cart_handler_set_meta_entry(Cart *cart, const char *entry, const char *new_value) {
    if (cart == NULL || cart->metadata == NULL) {
        return -1; 
    }

    if (strcmp(entry, "name") == 0) {
        strncpy(cart->metadata->name, new_value, MAX_STR_LEN - 1);
        cart->metadata->name[MAX_STR_LEN - 1] = '\0';
    } else if (strcmp(entry, "description") == 0) {
        strncpy(cart->metadata->description, new_value, MAX_STR_LEN - 1);
        cart->metadata->description[MAX_STR_LEN - 1] = '\0';
    } else if (strcmp(entry, "author") == 0) {
        strncpy(cart->metadata->author, new_value, MAX_STR_LEN - 1);
        cart->metadata->author[MAX_STR_LEN - 1] = '\0';
    } else if (strcmp(entry, "created") == 0) {
        strncpy(cart->metadata->created, new_value, MAX_DATE_LEN - 1);
        cart->metadata->created[MAX_DATE_LEN - 1] = '\0';
    } else if (strcmp(entry, "deadline") == 0) {
        strncpy(cart->metadata->deadline, new_value, MAX_DATE_LEN - 1);
        cart->metadata->deadline[MAX_DATE_LEN - 1] = '\0';
    } else if (strcmp(entry, "version") == 0) {
        strncpy(cart->metadata->version, new_value, MAX_VERSION_LEN - 1);
        cart->metadata->version[MAX_VERSION_LEN - 1] = '\0';
    } else {
        print_colored(ERROR_COLOR, "The metadata entry does not exist!");
        return -1; 
    }

    return 0;
}

// Function to get a metadata entry from the CART structure
int cart_handler_get_meta_entry(Cart *cart, const char *entry, char *value) {
    if (cart == NULL || cart->metadata == NULL) {
        return -1; 
    }

    if (strcmp(entry, "name") == 0) {
        strncpy(value, cart->metadata->name, MAX_STR_LEN - 1);
        value[MAX_STR_LEN - 1] = '\0';
    } else if (strcmp(entry, "description") == 0) {
        strncpy(value, cart->metadata->description, MAX_STR_LEN - 1);
        value[MAX_STR_LEN - 1] = '\0';
    } else if (strcmp(entry, "author") == 0) {
        strncpy(value, cart->metadata->author, MAX_STR_LEN - 1);
        value[MAX_STR_LEN - 1] = '\0';
    } else if (strcmp(entry, "created") == 0) {
        strncpy(value, cart->metadata->created, MAX_DATE_LEN - 1);
        value[MAX_DATE_LEN - 1] = '\0';
    } else if (strcmp(entry, "deadline") == 0) {
        strncpy(value, cart->metadata->deadline, MAX_DATE_LEN - 1);
        value[MAX_DATE_LEN - 1] = '\0';
    } else if (strcmp(entry, "version") == 0) {
        strncpy(value, cart->metadata->version, MAX_VERSION_LEN - 1);
        value[MAX_VERSION_LEN - 1] = '\0';
    } else {
        print_colored(ERROR_COLOR, "The metadata entry does not exist!");
        return -1; 
    }

    return 0;
}

// Function to list all metadata entries in the CART structure
int cart_handler_list_meta(Cart *cart) {
    if (cart != NULL && cart->metadata != NULL) {
        print_colored(BLUE_COLOR, "🗂  Project Metadata:\n");
        print_colored(GREEN_COLOR, "🔹 %-12s: %s\n", "Name", cart->metadata->name);
        print_colored(GREEN_COLOR, "🔹 %-12s: %s\n", "Author", cart->metadata->author);
        print_colored(GREEN_COLOR, "🔹 %-12s: %s\n", "Description", cart->metadata->description);
        print_colored(GREEN_COLOR, "🔹 %-12s: %s\n", "Version", cart->metadata->version);
        print_colored(GREEN_COLOR, "🔹 %-12s: %s\n", "Created", cart->metadata->created);
        print_colored(GREEN_COLOR, "🔹 %-12s: %s\n", "Deadline", cart->metadata->deadline);
        
    } else {
        print_colored(ERROR_COLOR, "Metadata not found in your project my friend!");
        return -1;
    }
    return 0;
}

// Find cart file implementation that can be overridden by tests
__attribute__((weak)) int find_cart_file(char *filename, size_t size) {
    // If function pointer is set (e.g. by tests), use that implementation
    if (find_cart_file_func != NULL) {
        return find_cart_file_func(filename, size);
    }

    // Default implementation
    DIR *d = opendir(".");
    if (d) {
        struct dirent *dir;
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, ".cart") != NULL) {
                strncpy(filename, dir->d_name, size - 1);
                filename[size - 1] = '\0';
                closedir(d);
                return 0;
            }
        }
        closedir(d);
    }
    return -1;
}

// Function to convert a CART structure to an XML document
xmlDocPtr cart_to_doc(const Cart *cart) {
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "project");
    xmlDocSetRootElement(doc, root_node);

    xmlNodePtr metadata_node = xmlNewChild(root_node, NULL, BAD_CAST "metadata", NULL);
    xmlNewChild(metadata_node, NULL, BAD_CAST "name", BAD_CAST cart->metadata->name);
    xmlNewChild(metadata_node, NULL, BAD_CAST "description", BAD_CAST cart->metadata->description);
    xmlNewChild(metadata_node, NULL, BAD_CAST "author", BAD_CAST cart->metadata->author);
    xmlNewChild(metadata_node, NULL, BAD_CAST "created", BAD_CAST cart->metadata->created);
    xmlNewChild(metadata_node, NULL, BAD_CAST "version", BAD_CAST cart->metadata->version);
    xmlNewChild(metadata_node, NULL, BAD_CAST "deadline", BAD_CAST cart->metadata->deadline);

    xmlNodePtr features_node = xmlNewChild(root_node, NULL, BAD_CAST "features", NULL);
    for (int i = 0; i < cart->num_features; i++) {
        xmlNodePtr feature_node = xmlNewChild(features_node, NULL, BAD_CAST "feature", NULL);
        xmlNewProp(feature_node, BAD_CAST "id", BAD_CAST cart->features[i]->id);
        xmlNewProp(feature_node, BAD_CAST "alias", BAD_CAST cart->features[i]->alias);
        xmlNewChild(feature_node, NULL, BAD_CAST "name", BAD_CAST cart->features[i]->name);
        xmlNewChild(feature_node, NULL, BAD_CAST "description", BAD_CAST cart->features[i]->description);
        xmlNewChild(feature_node, NULL, BAD_CAST "status", BAD_CAST status_to_string(cart->features[i]->status));
        xmlNewChild(feature_node, NULL, BAD_CAST "priority", BAD_CAST priority_to_string(cart->features[i]->priority));

        xmlNodePtr tags_node = xmlNewChild(feature_node, NULL, BAD_CAST "tags", NULL);
        for (int j = 0; j < cart->features[i]->num_tags; j++) {
            xmlNewChild(tags_node, NULL, BAD_CAST "tag", BAD_CAST cart->features[i]->tags[j]);
        }
    }

    return doc;
}

// Function to convert an XML document to a CART structure
Cart *doc_to_cart(xmlDocPtr doc) {
    Cart *cart = (Cart *)malloc(sizeof(Cart));
    memset(cart, 0, sizeof(Cart));

    cart->metadata = (Metadata *)malloc(sizeof(Metadata));
    memset(cart->metadata, 0, sizeof(Metadata));

    cart->features = (Feature **)malloc(MAX_FEATURES * sizeof(Feature *));
    for (int i = 0; i < MAX_FEATURES; i++) {
        cart->features[i] = NULL;
    }

    xmlNodePtr root_node = xmlDocGetRootElement(doc);
    if (root_node == NULL) {
        print_colored(ERROR_COLOR, "Empty XML document!\n");
        return NULL;
    }

    xmlNodePtr current_node = root_node->children;
    while (current_node != NULL) {
        if (strcmp((const char *)current_node->name, "metadata") == 0) {
            xmlNodePtr metadata_node = current_node->children;
            while (metadata_node != NULL) {
                if (strcmp((const char *)metadata_node->name, "name") == 0) {
                    strncpy(cart->metadata->name, (const char *)xmlNodeGetContent(metadata_node), sizeof(cart->metadata->name) - 1);
                } else if (strcmp((const char *)metadata_node->name, "description") == 0) {
                    strncpy(cart->metadata->description, (const char *)xmlNodeGetContent(metadata_node), sizeof(cart->metadata->description) - 1);
                } else if (strcmp((const char *)metadata_node->name, "author") == 0) {
                    strncpy(cart->metadata->author, (const char *)xmlNodeGetContent(metadata_node), sizeof(cart->metadata->author) - 1);
                } else if (strcmp((const char *)metadata_node->name, "created") == 0) {
                    strncpy(cart->metadata->created, (const char *)xmlNodeGetContent(metadata_node), sizeof(cart->metadata->created) - 1);
                } else if (strcmp((const char *)metadata_node->name, "version") == 0) {
                    strncpy(cart->metadata->version, (const char *)xmlNodeGetContent(metadata_node), sizeof(cart->metadata->version) - 1);
                } else if (strcmp((const char *)metadata_node->name, "deadline") == 0) {
                    strncpy(cart->metadata->deadline, (const char *)xmlNodeGetContent(metadata_node), sizeof(cart->metadata->deadline) - 1);
                }
                metadata_node = metadata_node->next;
            }
        } else if (strcmp((const char *)current_node->name, "features") == 0) {
            xmlNodePtr feature_node = current_node->children;
            while (feature_node != NULL && cart->num_features < MAX_FEATURES) {
                if (strcmp((const char *)feature_node->name, "feature") == 0) {
                    Feature *feature = (Feature *)malloc(sizeof(Feature));
                    memset(feature, 0, sizeof(Feature));
                    cart->features[cart->num_features++] = feature;

                    xmlChar *id = xmlGetProp(feature_node, BAD_CAST "id");
                    strncpy(feature->id, (const char *)id, sizeof(feature->id) - 1);
                    xmlFree(id);

                    mark_id_as_used(feature->id);

                    xmlChar *alias = xmlGetProp(feature_node, BAD_CAST "alias");
                    strncpy(feature->alias, (const char *)alias, sizeof(feature->alias) - 1);
                    xmlFree(alias);

                    xmlNodePtr feature_detail = feature_node->children;
                    while (feature_detail != NULL) {
                        if (strcmp((const char *)feature_detail->name, "name") == 0) {
                            strncpy(feature->name, (const char *)xmlNodeGetContent(feature_detail), sizeof(feature->name) - 1);
                        } else if (strcmp((const char *)feature_detail->name, "description") == 0) {
                            strncpy(feature->description, (const char *)xmlNodeGetContent(feature_detail), sizeof(feature->description) - 1);
                        } else if (strcmp((const char *)feature_detail->name, "status") == 0) {
                            feature->status = string_to_status((const char *)xmlNodeGetContent(feature_detail));
                        } else if (strcmp((const char *)feature_detail->name, "priority") == 0) {
                            feature->priority = string_to_priority((const char *)xmlNodeGetContent(feature_detail));
                        } else if (strcmp((const char *)feature_detail->name, "tags") == 0) {
                            xmlNodePtr tag_node = feature_detail->children;
                            while (tag_node != NULL && feature->num_tags < MAX_TAGS) {
                                if (strcmp((const char *)tag_node->name, "tag") == 0) {
                                    xmlChar *content = xmlNodeGetContent(tag_node);
                                    if (content) {
                                        strncpy(feature->tags[feature->num_tags], (const char *)content, 
                                                sizeof(feature->tags[feature->num_tags]) - 1);
                                        feature->tags[feature->num_tags][sizeof(feature->tags[feature->num_tags]) - 1] = '\0';
                                        xmlFree(content);
                                        feature->num_tags++;
                                    }
                                }
                                tag_node = tag_node->next;
                            }
                        }
                        feature_detail = feature_detail->next;
                    }
                }
                feature_node = feature_node->next;
            }
        }
        current_node = current_node->next;
    }

    return cart;
}

// Function to write a CART structure to a CART file
int cart_handler_write_project(CartHandler *handler, const Cart *cart) {
    if (handler == NULL || cart == NULL) {
        return -1;
    }
    handler->doc = cart_to_doc(cart);
    return handler->doc ? 0 : -1;
}

// Function to read a CART structure from a CART file
int cart_handler_read_project(CartHandler *handler, Cart *cart) {
    if (handler == NULL || cart == NULL || handler->doc == NULL) {
        return -1;
    }

    // Free existing cart contents first
    free_cart(cart);

    // Parse the XML document into a new cart structure
    Cart *parsed_cart = doc_to_cart(handler->doc);
    if (parsed_cart == NULL) {
        return -1;
    }

    // Allocate new memory for the destination cart
    cart->metadata = (Metadata *)malloc(sizeof(Metadata));
    if (!cart->metadata) {
        free_cart(parsed_cart);
        free(parsed_cart);
        return -1;
    }
    memcpy(cart->metadata, parsed_cart->metadata, sizeof(Metadata));

    // Allocate and copy features
    cart->features = (Feature **)malloc(MAX_FEATURES * sizeof(Feature *));
    if (!cart->features) {
        free(cart->metadata);
        cart->metadata = NULL;
        free_cart(parsed_cart);
        free(parsed_cart);
        return -1;
    }

    // Initialize all feature pointers to NULL
    for (int i = 0; i < MAX_FEATURES; i++) {
        cart->features[i] = NULL;
    }

    // Copy each feature
    cart->num_features = parsed_cart->num_features;
    for (int i = 0; i < parsed_cart->num_features; i++) {
        cart->features[i] = (Feature *)malloc(sizeof(Feature));
        if (!cart->features[i]) {
            free_cart(cart);
            free_cart(parsed_cart);
            free(parsed_cart);
            return -1;
        }
        
        // Copy scalar fields explicitly using correct field names
        strncpy(cart->features[i]->id, parsed_cart->features[i]->id, sizeof(cart->features[i]->id) - 1);
        cart->features[i]->id[sizeof(cart->features[i]->id) - 1] = '\0'; // Ensure null termination
        
        strncpy(cart->features[i]->name, parsed_cart->features[i]->name, sizeof(cart->features[i]->name) - 1);
        cart->features[i]->name[sizeof(cart->features[i]->name) - 1] = '\0';
        
        strncpy(cart->features[i]->description, parsed_cart->features[i]->description, sizeof(cart->features[i]->description) - 1);
        cart->features[i]->description[sizeof(cart->features[i]->description) - 1] = '\0';
        
        // Copy alias field
        strncpy(cart->features[i]->alias, parsed_cart->features[i]->alias, sizeof(cart->features[i]->alias) - 1);
        cart->features[i]->alias[sizeof(cart->features[i]->alias) - 1] = '\0';
        
        // Copy enum values
        cart->features[i]->status = parsed_cart->features[i]->status;
        cart->features[i]->priority = parsed_cart->features[i]->priority;
        
        // Copy tags carefully
        cart->features[i]->num_tags = parsed_cart->features[i]->num_tags;
        for (int j = 0; j < parsed_cart->features[i]->num_tags; j++) {
            strncpy(cart->features[i]->tags[j], parsed_cart->features[i]->tags[j], sizeof(cart->features[i]->tags[j]) - 1);
            cart->features[i]->tags[j][sizeof(cart->features[i]->tags[j]) - 1] = '\0'; // Ensure null termination
        }
    }

    // Clean up the temporary parsed cart
    free_cart(parsed_cart);
    free(parsed_cart);
    
    return 0;
}

// Function to convert a status enum to a string
const char *status_to_string(Status status) {
    switch (status) {
        case STATUS_OPEN:
            return "OPEN";
        case STATUS_DONE:
            return "DONE";
        case STATUS_IN_PROGRESS:
            return "IN PROGRESS";
        case STATUS_UNKNOWN:
            return "UNKNOWN";
        default:
            return "UNKNOWN";
    }
}

// Function to convert a string to a status enum
Status string_to_status(const char *status_str) {
    if (strcmp(status_str, "DONE") == 0) {
        return STATUS_DONE;
    } else if (strcmp(status_str, "OPEN") == 0) {
        return STATUS_OPEN;
    } else if (strcmp(status_str, "IN PROGRESS") == 0) {
        return STATUS_IN_PROGRESS;
    } else if (strcmp(status_str, "UNKNOWN") == 0) {
        return STATUS_UNKNOWN;
    } else {
        return STATUS_UNKNOWN;
    }
}

// Function to convert a priority enum to a string
const char *priority_to_string(Priority priority) {
    switch (priority) {
        case PRIORITY_LOW:
            return "LOW";
        case PRIORITY_MEDIUM:
            return "MEDIUM";
        case PRIORITY_HIGH:
            return "HIGH";
        case PRIORITY_CRITICAL:
            return "CRITICAL";
        case PRIORITY_UNKNOWN:
            return "UNKNOWN";
        default:
            return "UNKNOWN";
    }
}

// Function to convert a string to a priority enum
Priority string_to_priority(const char *priority_str) {
    if (strcmp(priority_str, "LOW") == 0) {
        return PRIORITY_LOW;
    } else if (strcmp(priority_str, "MEDIUM") == 0) {
        return PRIORITY_MEDIUM;
    } else if (strcmp(priority_str, "HIGH") == 0) {
        return PRIORITY_HIGH;
    } else if (strcmp(priority_str, "CRITICAL") == 0) {
        return PRIORITY_CRITICAL;
    } else if (strcmp(priority_str, "UNKNOWN") == 0) {
        return PRIORITY_UNKNOWN;
    } else {
        return STATUS_UNKNOWN;
    }
}

// Function to free the memory allocated for a CART structure
void free_cart(Cart *cart) {
    if (cart == NULL) {
        return;
    }
    
    if (cart->metadata != NULL) {
        free(cart->metadata);
        cart->metadata = NULL;
    }
    if (cart->features != NULL) {
        for (int i = 0; i < cart->num_features; i++) {
            if (cart->features[i] != NULL) {
                free(cart->features[i]);
                cart->features[i] = NULL;
            }
        }
        free(cart->features);
        cart->features = NULL;
    }
}