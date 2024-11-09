#ifndef FEATURE_H
#define FEATURE_H

#include <string.h>

int cmd_feature(int argc, char *argv[]);
int cmd_feature_add(int argc, char *argv[]);
int cmd_feature_get(int argc, char *argv[]);
int cmd_feature_remove(int argc, char *argv[]);
int cmd_feature_update(int argc, char *argv[]);

#endif // FEATURE_H