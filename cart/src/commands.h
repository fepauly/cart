#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

// Structure to represent a command
typedef struct {
    const char *name; // Name of the command
    int (*function)(int, char**); // Function pointer to the command handler
    const char *help; // Help text for the command
} Command;

// Array of main commands
extern Command commands[];
// Array of meta commands
extern Command meta_commands[];
// Array of deadline commands
extern Command deadline_commands[];
// Array of feature commands
extern Command feature_commands[];

#ifdef __cplusplus
}
#endif

#endif