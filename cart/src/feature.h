#ifndef FEATURE_H
#define FEATURE_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Function to handle the 'feature' command
int cmd_feature(int argc, char *argv[]);
// Function to handle the 'feature add' command
int cmd_feature_add(int argc, char *argv[]);
// Function to handle the 'feature get' command
int cmd_feature_get(int argc, char *argv[]);
// Function to handle the 'feature remove' command
int cmd_feature_remove(int argc, char *argv[]);
// Function to handle the 'feature update' command
int cmd_feature_update(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif // FEATURE_H