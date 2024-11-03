#include "commands.h"
#include "init.h"
#include "meta.h"

Command commands[] = {
    {"init", cmd_init, "Initialize your CART project!"},
    {"meta", cmd_meta, "Initialize your CART project!"},
    {NULL, NULL, NULL}
};

Command meta_commands[] = {
    {"update", cmd_meta_update, "Update a metadata entry. Usage: cart meta update <field> <value>"},
    {"get", cmd_meta_get, "Get a metadata entry. Usage: cart meta get <field>"},
    {"list", cmd_meta_list, "List all metadata entry. Usage: cart meta list"},
    {NULL, NULL, NULL}
};