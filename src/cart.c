#include "cart.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "pretty.h"

int cart_handler_open(CartHandler *handler, const char *filename, const char *mode) {
    if (strcmp(mode, "w") == 0) { // write
        handler->doc = xmlNewDoc(BAD_CAST "1.0");
        handler->root_node = xmlNewNode(NULL, BAD_CAST "project");
        xmlDocSetRootElement(handler->doc, handler->root_node);
    } else if (strcmp(mode, "r+") == 0) { // read and write
        handler->doc = xmlParseFile(filename);
        if(handler->doc == NULL) {
            print_colored(ERROR_COLOR, "Failed to parse %s.\n", filename);
            return -1;
        }

        handler->root_node = xmlDocGetRootElement(handler->doc);
        if (handler->root_node == NULL) {
            print_colored(ERROR_COLOR, "The project is empty my friend! (file: %s)", filename);
            xmlFreeDoc(handler->doc);
            handler->doc = NULL;
            return -1;
        } 
    } else {
        print_colored(ERROR_COLOR, "Invalid file mode: %s\n", mode);
        return -1;
    }
    return 0;
}

int cart_handler_close(CartHandler *handler) {
    if(handler->doc) {
        xmlFreeDoc(handler->doc);
        handler->doc = NULL;
        return 0;
    }
    print_colored(ERROR_COLOR, "File already closed!");
    return -1;
}

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

int cart_handler_init_metadata(CartHandler *handler, Metadata *metadata) {
    if (handler == NULL || metadata == NULL || handler->doc == NULL) {
        return -1;
    }
    
    xmlNodePtr metadata_node = xmlNewNode(NULL, BAD_CAST "metadata");
    xmlAddChild(handler->root_node, metadata_node);

    xmlNewChild(metadata_node, NULL, BAD_CAST "name", BAD_CAST metadata->name);
    xmlNewChild(metadata_node, NULL, BAD_CAST "description", BAD_CAST metadata->description);
    xmlNewChild(metadata_node, NULL, BAD_CAST "author", BAD_CAST metadata->author);
    if (strlen(metadata->version) > 0) {
        xmlNewChild(metadata_node, NULL, BAD_CAST "version", BAD_CAST metadata->version);
    } else {
        xmlNewChild(metadata_node, NULL, BAD_CAST "version", BAD_CAST "1.0");
    }
    xmlNewChild(metadata_node, NULL, BAD_CAST "created", BAD_CAST metadata->created);
    xmlNewChild(metadata_node, NULL, BAD_CAST "deadline", BAD_CAST metadata->deadline);

    return 0;
}

int cart_handler_init_features(CartHandler *handler) {
    if (handler == NULL || handler->doc == NULL) {
        return -1;
    }
    xmlNodePtr features_node = xmlNewNode(NULL, BAD_CAST "features");
    xmlAddChild(handler->root_node, features_node);

    return 0;
}

int cart_handler_set_meta_entry(CartHandler *handler, const char *entry, const char *new_value) {
    xmlNode *currentNode = handler->root_node->children;
    xmlNode *metadata_node = NULL;
    // Get metadata node
    while (currentNode != NULL) {
        if (strcmp((const char*)currentNode->name, "metadata") == 0) {
            metadata_node = currentNode;
            break;
        }
        currentNode = currentNode->next;
    }
    int updated = 0;
    if (metadata_node != NULL) {
        currentNode = metadata_node->children;
        while (currentNode != NULL) {
            if (strcmp((const char *)currentNode->name, entry) == 0) {
                xmlNodeSetContent(currentNode, BAD_CAST new_value);
                updated = 1;
                break;
            }
            currentNode = currentNode->next;
        }
    } else {
        print_colored(ERROR_COLOR, "Metadata node not found in your project my friend!");
        return -1;
    }
    return updated == 1 ? 0 : -1;
}

int cart_handler_get_meta_entry(CartHandler *handler, const char *entry, char *value, size_t value_size) {
    xmlNode *currentNode = handler->root_node->children;
    xmlNode *metadata_node = NULL;
    // Get metadata node
    while (currentNode != NULL) {
        if (strcmp((const char*)currentNode->name, "metadata") == 0) {
            metadata_node = currentNode;
            break;
        }
        currentNode = currentNode->next;
    }
    int entry_found = 0;
    if (metadata_node != NULL) {
        currentNode = metadata_node->children;
        while (currentNode != NULL) {
            if (strcmp((const char *)currentNode->name, entry) == 0) {
                char* content = (char *)xmlNodeGetContent(currentNode);
                if (content) {
                    strncpy(value, content, value_size - 1);
                    value[value_size - 1] = '\0';
                    xmlFree(content);
                    entry_found = 1;
                } else {
                    return -1;
                }
                break;
            }
            currentNode = currentNode->next;
        }
    } else {
        print_colored(ERROR_COLOR, "Metadata node not found in your project my friend!");
        return -1;
    }
    return entry_found == 1 ? 0 : -1;
}

int cart_handler_list_meta(CartHandler *handler) {
    xmlNode *currentNode = handler->root_node->children;
    xmlNode *metadata_node = NULL;
    // Get metadata node
    while (currentNode != NULL) {
        if (strcmp((const char*)currentNode->name, "metadata") == 0) {
            metadata_node = currentNode;
            break;
        }
        currentNode = currentNode->next;
    }
    if (metadata_node != NULL) {
        print_colored(BLUE_COLOR, "🗂  Project Metadata:\n");
        currentNode = metadata_node->children;
        while (currentNode != NULL) {
            if (currentNode->type == XML_ELEMENT_NODE) {
                char* content = (char *)xmlNodeGetContent(currentNode);
                if (content) {
                    print_colored(GREEN_COLOR, "🔹 %-12s: %s\n", (const char *)currentNode->name, content);
                    xmlFree(content);
                } else {
                    print_colored(ERROR_COLOR, "Missing content in node %s", (char *)currentNode->name);
                }
            }
            currentNode = currentNode->next;
        }
    } else {
        print_colored(ERROR_COLOR, "Metadata node not found in your project my friend!");
        return -1;
    }
    return 0;
}

int find_cart_file(char *filename, size_t size) {
    DIR *d;

    d = opendir(".");
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