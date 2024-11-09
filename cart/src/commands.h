#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>

typedef struct {
    const char *name;
    int (*function)(int, char**);
    const char *help;
} Command;

extern Command commands[];
extern Command meta_commands[];
extern Command deadline_commands[];
extern Command feature_commands[];

#endif