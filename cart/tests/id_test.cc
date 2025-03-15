#include <gtest/gtest.h>
#include "cart_handler/id.h"
#include <string>
#include <unordered_set>

class IDTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Re-initialize the ID system for each test
        init_id_system();
    }
};

// Test unique ID generation
TEST_F(IDTest, GeneratesUniqueIDs) {
    const int NUM_IDS = 50; // Test with a reasonable number of IDs
    std::unordered_set<std::string> ids;
    
    for (int i = 0; i < NUM_IDS; i++) {
        char id_buffer[MAX_ID_LEN];
        EXPECT_EQ(generate_unique_id(id_buffer, MAX_ID_LEN), 0) << "ID generation failed";
        
        // Ensure the ID is correctly formatted (3 digits)
        EXPECT_EQ(strlen(id_buffer), 3) << "ID length is not 3 digits";
        
        // Ensure the ID is unique
        std::string id_str(id_buffer);
        EXPECT_TRUE(ids.find(id_str) == ids.end()) << "Duplicate ID generated: " << id_str;
        
        ids.insert(id_str);
    }
}

// Test bit operations
TEST_F(IDTest, BitOperations) {
    unsigned int bitset[BITSET_SIZE] = {0};
    
    // Set and check some bits
    set_id_bit(bitset, 0);
    set_id_bit(bitset, 42);
    set_id_bit(bitset, 99);
    
    EXPECT_TRUE(is_id_bit_set(bitset, 0));
    EXPECT_TRUE(is_id_bit_set(bitset, 42));
    EXPECT_TRUE(is_id_bit_set(bitset, 99));
    EXPECT_FALSE(is_id_bit_set(bitset, 1));
    EXPECT_FALSE(is_id_bit_set(bitset, 41));
    
    // Clear bits and verify
    clear_id_bit(bitset, 42);
    EXPECT_TRUE(is_id_bit_set(bitset, 0));
    EXPECT_FALSE(is_id_bit_set(bitset, 42));
    EXPECT_TRUE(is_id_bit_set(bitset, 99));
}

// Test ID marking and releasing
TEST_F(IDTest, MarkAndReleaseIDs) {
    // Mark IDs as used
    mark_id_as_used("001");
    mark_id_as_used("042");
    mark_id_as_used("099");
    
    // Try to generate a new ID - it should not be any of the marked ones
    char id_buffer[MAX_ID_LEN];
    EXPECT_EQ(generate_unique_id(id_buffer, MAX_ID_LEN), 0);
    
    EXPECT_STRNE(id_buffer, "001");
    EXPECT_STRNE(id_buffer, "042");
    EXPECT_STRNE(id_buffer, "099");
    
    // Release an ID and verify it can be reused
    release_id("042");
    
    // Now reset the ID system and mark only 001 and 099
    init_id_system();
    mark_id_as_used("001");
    mark_id_as_used("099");
    
    // Generate a few IDs - at some point we should get 042 since it's available
    bool found_042 = false;
    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(generate_unique_id(id_buffer, MAX_ID_LEN), 0);
        if (strcmp(id_buffer, "042") == 0) {
            found_042 = true;
            break;
        }
    }
    
    // Given enough tries, we should get 042 eventually
    EXPECT_TRUE(found_042) << "Released ID 042 was not reused";
}