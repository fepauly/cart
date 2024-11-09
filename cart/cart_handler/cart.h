#ifndef CART_H
#define CART_H

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define MAX_FEATURES 100
#define MAX_REQUIREMENTS 30
#define MAX_TAGS 5
#define MAX_STR_LEN 256
#define MAX_DATE_LEN 11
#define MAX_VERSION_LEN 10
#define MAX_ID_LEN 3

typedef struct {
    char name[MAX_STR_LEN];
    char description[MAX_STR_LEN];
    char author[MAX_STR_LEN];
    char created[MAX_DATE_LEN]; // e.g. "YYYY-MM-DD"
    char deadline[MAX_DATE_LEN]; // e.g. "YYYY-MM-DD"
    char version[MAX_VERSION_LEN];
} Metadata;

typedef enum {
    STATUS_OPEN,
    STATUS_IN_PROGRESS,
    STATUS_DONE,
    STATUS_UNKNOWN
} Status;

typedef enum {
    PRIORITY_LOW,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH,
    PRIORITY_CRITICAL,
    PRIORITY_UNKNOWN
} Priority;

typedef struct {
    char id[MAX_ID_LEN];
    char name[MAX_STR_LEN];
    char description[MAX_STR_LEN];
    char alias[MAX_STR_LEN];
    Status status;
    char tags[MAX_TAGS][MAX_STR_LEN];
    int num_tags;
    char deadline[MAX_DATE_LEN];
    Priority priority;
} Feature;

typedef struct {
    Metadata *metadata;
    Feature **features;
    int num_features;
} Cart; 


typedef struct CartHandler CartHandler; 

struct CartHandler{
    xmlDocPtr doc;
};

int cart_handler_open(CartHandler *handler, const char *filename);
int cart_handler_close(CartHandler *handler);
int find_cart_file(char *filename, size_t size);
int cart_handler_save(CartHandler *handler, const char *filename);
int cart_handler_init_features(CartHandler *handler);
int cart_handler_set_meta_entry(Cart *cart, const char *entry, const char *new_value);
int cart_handler_get_meta_entry(Cart *cart, const char *entry, char *value);
int cart_handler_list_meta(Cart *cart);
xmlDocPtr cart_to_doc(const Cart *cart);
Cart *doc_to_cart(xmlDocPtr doc);
int cart_handler_write_project(CartHandler *handler, const Cart *cart);
int cart_handler_read_project(CartHandler *handler, Cart *cart);

const char *status_to_string(Status status);
Status string_to_status(const char *status_str);
Priority string_to_priority(const char *priority_str);
const char *priority_to_string(Priority priority);
void free_cart(Cart *cart);

#endif