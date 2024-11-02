#include "cart.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>

int cart_handler_open(CartHandler *handler, const char *filename, const char *mode) {
    handler->file = fopen(filename, mode);
    return (handler->file != NULL) ? 0 : -1;
}

int cart_handler_close(CartHandler *handler) {
    if(handler->file) {
        fclose(handler->file);
        handler->file = NULL;
        return 0;
    }
    return -1;
}

int cart_handler_init_project_section(CartHandler *handler, Project *project) {
    if (handler == NULL || project == NULL || handler->file == NULL) {
        return -1;
    }
    
    fprintf(handler->file, "[Project]\n");
    fprintf(handler->file, "Name: %s\n", project->name);
    fprintf(handler->file, "Description: %s\n", project->description);
    fprintf(handler->file, "Author: %s\n", project->author);
    fprintf(handler->file, "Created: %s\n", project->created);
    if (strlen(project->version) > 0) {
        fprintf(handler->file, "Version: %s\n", project->version);
    } else {
        fprintf(handler->file, "Version: 1.0\n");
    }
    return 0;
}

int find_cart_file(char *filename, size_t size) {
    DIR *d;
    struct dirent *dir;

    d = opendir(".");
    if (d) {
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