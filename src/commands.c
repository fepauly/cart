#include "commands.h"
#include "init.h"

Command commands[] = {
    {"init", cmd_init, "Initialize your CART project!"},
    {NULL, NULL, NULL}
};