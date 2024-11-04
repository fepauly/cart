#include "commands.h"
#include "init.h"
#include "meta.h"
#include "deadline.h"

Command commands[] = {
    {"init", cmd_init, "Initialize your CART project!"},
    {"meta", cmd_meta, "Set and get metadata!"},
    {"deadline", cmd_deadline, "Set, get and check the deadline of your project and features!"},
    {NULL, NULL, NULL}
};

Command meta_commands[] = {
    {"set", cmd_meta_set, "set a metadata entry. Usage: cart meta set <field> <value>"},
    {"get", cmd_meta_get, "Get a metadata entry. Usage: cart meta get <field>"},
    {"list", cmd_meta_list, "List all metadata entry. Usage: cart meta list"},
    {NULL, NULL, NULL}
};

Command deadline_commands[] = {
    {"set", cmd_deadline_set, "set the deadline. Usage: cart deadline set -d <day> -m <month> -y <year>"},
    {"get", cmd_deadline_get, "Get the project deadline. Usage: cart deadline get"},
    {"check", cmd_deadline_check, "Check the project deadline and get the days left until deadline. Usage: cart deadline check"},
    {NULL, NULL, NULL}
};