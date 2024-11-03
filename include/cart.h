#ifndef CART_H
#define CART_H

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define MAX_FEATURES 100
#define MAX_REQUIREMENTS 30
#define MAX_TAGS 5

typedef struct {
    char name[250];
    char description[512];
    char author[256];
    char created[11]; // e.g. "YYYY-MM-DD"
    char deadline[11]; // e.g. "YYYY-MM-DD"
    char version[10];
} Metadata;

typedef struct {
    char title[256];
    char category[50];
    char description[512];
    char status[50];
} Requirement;

typedef struct {
    char tag[50];
} Tag;

typedef struct {
    char title[256];
    char status[50];
    Requirement requirements[MAX_REQUIREMENTS];
    int num_requirements;
    Tag tags[MAX_TAGS];
    int num_tags;
    char deadline[20];
    int priority;
} Feature;

typedef struct {
    Metadata metadata;
    Feature features[MAX_FEATURES];
    int num_features;
} Cart; 


typedef struct CartHandler CartHandler; 

struct CartHandler{
    xmlDocPtr doc;
    xmlNodePtr root_node;
};

int cart_handler_open(CartHandler *handler, const char *filename, const char *mode);
int cart_handler_close(CartHandler *handler);
int cart_handler_init_metadata(CartHandler *handler, Metadata *metadata);
int find_cart_file(char *filename, size_t size);
int cart_handler_save(CartHandler *handler, const char *filename);
int cart_handler_init_features(CartHandler *handler);
int cart_handler_set_meta_entry(CartHandler *handler, const char *entry, const char *new_value);
int cart_handler_get_meta_entry(CartHandler *handler, const char *entry, char *value, size_t value_size);
int cart_handler_list_meta(CartHandler *handler);

#endif