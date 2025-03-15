#ifndef ID_H
#define ID_H

#include <stddef.h>
#include "cart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BITSET_SIZE ((MAX_FEATURES / (sizeof(unsigned int) * 8)) + 1)

void set_id_bit(unsigned int *bitset, int id_bit);
int is_id_bit_set(unsigned int *bitset, int id_bit);
void clear_id_bit(unsigned int *bitset, int id_bit);
void init_id_system();
int generate_unique_id(char *id_buffer, size_t buffer_size);
void mark_id_as_used(const char *id);
void release_id(const char *id);

#ifdef __cplusplus
}
#endif

#endif