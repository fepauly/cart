#include "id.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static unsigned int id_bitset[BITSET_SIZE];
static int initialized = 0;

// Function to check if a bit is set in the ID bitset
int is_id_bit_set(unsigned int *bitset, int id_bit) {
    return (bitset[id_bit / (sizeof(unsigned int) * 8)] & 
           (1 << (id_bit % (sizeof(unsigned int) * 8)))) != 0;
}

// Function to set a bit in the ID bitset
void set_id_bit(unsigned int *bitset, int id_bit) {
    bitset[id_bit / (sizeof(unsigned int) * 8)] |= (1 << (id_bit % (sizeof(unsigned int) * 8)));
}

// Function to clear a bit in the ID bitset
void clear_id_bit(unsigned int *bitset, int id_bit) {
    bitset[id_bit / (sizeof(unsigned int) * 8)] &= ~(1 << (id_bit % (sizeof(unsigned int) * 8)));
}

// Function to initialize the ID system
void init_id_system() {
    if (!initialized) {
        for (int i = 0; i < BITSET_SIZE; i++) {
            id_bitset[i] = 0;
        }
        srand((unsigned int)time(NULL));
        initialized = 1;
    }
}

// Function to generate a unique ID for a feature
int generate_unique_id(char *id_buffer, size_t buffer_size) {
    init_id_system();
    
    if (buffer_size < MAX_ID_LEN) {
        return -1;
    }
    
    int id;
    do {
        id = rand() % MAX_FEATURES;
    } while (is_id_bit_set(id_bitset, id));
    
    set_id_bit(id_bitset, id);
    snprintf(id_buffer, buffer_size, "%03d", id);
    return 0;
}

// Function to mark an ID as used
void mark_id_as_used(const char *id) {
    init_id_system();
    
    int id_num = atoi(id);
    if (id_num >= 0 && id_num < MAX_FEATURES) {
        set_id_bit(id_bitset, id_num);
    }
}

// Function to release an ID when a feature is deleted
void release_id(const char *id) {
    int id_num = atoi(id);
    if (id_num >= 0 && id_num < MAX_FEATURES) {
        clear_id_bit(id_bitset, id_num);
    }
}