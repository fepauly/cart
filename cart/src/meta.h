#ifndef META_H
#define META_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Function to handle the 'meta' command
int cmd_meta(int argc, char *argv[]);
// Function to handle the 'meta set' command
int cmd_meta_set(int argc, char *argv[]);
// Function to handle the 'meta get' command
int cmd_meta_get(int argc, char *argv[]);
// Function to handle the 'meta list' command
int cmd_meta_list(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif // META_H