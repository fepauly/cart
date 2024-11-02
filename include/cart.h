#ifndef CART_H
#define CART_H

#include <stdio.h>

#define MAX_REQUIREMENTS 30

typedef struct {
    char name[250];
    char description[512];
    char author[256];
    char created[11]; // e.g. "YYYY-MM-DD"
    char version[10];
} Project;

typedef struct {
    char title[256];
    char category[50];
    char description[512];
    char status[50];
} Requirement;

typedef struct {
    char title[256];
    char status[50];
    Requirement requirements[MAX_REQUIREMENTS];
    int num_requirements;
    char deadline[20];
    int priority;
} Feature;

typedef struct CartHandler CartHandler; 

struct CartHandler{
    FILE* file;
};

int cart_handler_open(CartHandler *handler, const char *filename, const char *mode);
int cart_handler_close(CartHandler *handler);
int cart_handler_init_project_section(CartHandler *handler, Project *project);
int find_cart_file(char *filename, size_t size);

#endif