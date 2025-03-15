#include <gtest/gtest.h>
#include "cart_handler/cart.h"
#include <fstream>
#include <cstring>
#include <filesystem>

class CartHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up a test cart file
        testFilePath = "test_cart.cart";
        
        // Create a basic test cart file
        std::ofstream outFile(testFilePath);
        outFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                << "<project>\n"
                << "  <metadata>\n"
                << "    <name>test_project</name>\n"
                << "    <description>Test Description</description>\n"
                << "    <author>Test Author</author>\n"
                << "    <created>01/01/2023</created>\n"
                << "    <version>1.0.0</version>\n"
                << "    <deadline>12/31/2023</deadline>\n"
                << "  </metadata>\n"
                << "  <features/>\n"
                << "</project>\n";
        outFile.close();
    }

    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(testFilePath)) {
            std::filesystem::remove(testFilePath);
        }
    }

    std::string testFilePath;
};

// Test opening and closing a cart file
TEST_F(CartHandlerTest, OpenAndCloseCart) {
    CartHandler handler;
    
    // Test opening a valid cart file
    EXPECT_EQ(cart_handler_open(&handler, testFilePath.c_str()), 0);
    
    // Test closing the cart file
    EXPECT_EQ(cart_handler_close(&handler), 0);
    
    // Test opening a non-existent file
    EXPECT_NE(cart_handler_open(&handler, "nonexistent.cart"), 0);
}

// Test reading the project from a cart file
TEST_F(CartHandlerTest, ReadProject) {
    CartHandler handler;
    ASSERT_EQ(cart_handler_open(&handler, testFilePath.c_str()), 0);
    
    Cart cart = {0};
    EXPECT_EQ(cart_handler_read_project(&handler, &cart), 0);
    
    // Verify the metadata
    EXPECT_STREQ(cart.metadata->name, "test_project");
    EXPECT_STREQ(cart.metadata->description, "Test Description");
    EXPECT_STREQ(cart.metadata->author, "Test Author");
    EXPECT_STREQ(cart.metadata->created, "01/01/2023");
    EXPECT_STREQ(cart.metadata->version, "1.0.0");
    EXPECT_STREQ(cart.metadata->deadline, "12/31/2023");
    
    // Verify there are no features yet
    EXPECT_EQ(cart.num_features, 0);
    
    free_cart(&cart);
    cart_handler_close(&handler);
}

// Test writing and saving a cart project
TEST_F(CartHandlerTest, WriteAndSaveProject) {
    // Create a new cart structure
    Cart cart = {0};
    cart.metadata = (Metadata*)malloc(sizeof(Metadata));
    memset(cart.metadata, 0, sizeof(Metadata));
    
    // Set metadata
    strncpy(cart.metadata->name, "new_project", MAX_STR_LEN - 1);
    strncpy(cart.metadata->description, "New Description", MAX_STR_LEN - 1);
    strncpy(cart.metadata->author, "New Author", MAX_STR_LEN - 1);
    strncpy(cart.metadata->created, "02/02/2023", MAX_DATE_LEN - 1);
    strncpy(cart.metadata->version, "2.0.0", MAX_VERSION_LEN - 1);
    strncpy(cart.metadata->deadline, "02/02/2024", MAX_DATE_LEN - 1);
    
    // Create a feature
    cart.features = (Feature**)malloc(MAX_FEATURES * sizeof(Feature*));
    for (int i = 0; i < MAX_FEATURES; i++) {
        cart.features[i] = NULL;
    }
    
    Feature* feature = (Feature*)malloc(sizeof(Feature));
    memset(feature, 0, sizeof(Feature));
    
    strncpy(feature->id, "001", MAX_ID_LEN - 1);
    strncpy(feature->name, "Test Feature", MAX_STR_LEN - 1);
    strncpy(feature->description, "Feature Description", MAX_STR_LEN - 1);
    feature->status = STATUS_OPEN;
    feature->priority = PRIORITY_MEDIUM;
    
    cart.features[0] = feature;
    cart.num_features = 1;
    
    // Write and save the project
    CartHandler handler;
    EXPECT_EQ(cart_handler_write_project(&handler, &cart), 0);
    
    std::string newFilePath = "new_test.cart";
    EXPECT_EQ(cart_handler_save(&handler, newFilePath.c_str()), 0);
    
    // Check that the file was created
    EXPECT_TRUE(std::filesystem::exists(newFilePath));
    
    // Now read it back and verify the contents
    CartHandler newHandler;
    ASSERT_EQ(cart_handler_open(&newHandler, newFilePath.c_str()), 0);
    
    Cart readCart = {0};
    EXPECT_EQ(cart_handler_read_project(&newHandler, &readCart), 0);
    
    // Verify the metadata
    EXPECT_STREQ(readCart.metadata->name, "new_project");
    EXPECT_STREQ(readCart.metadata->author, "New Author");
    
    // Verify the feature
    ASSERT_EQ(readCart.num_features, 1);
    EXPECT_STREQ(readCart.features[0]->id, "001");
    EXPECT_STREQ(readCart.features[0]->name, "Test Feature");
    EXPECT_EQ(readCart.features[0]->status, STATUS_OPEN);
    
    // Clean up
    free_cart(&cart);
    free_cart(&readCart);
    cart_handler_close(&handler);
    cart_handler_close(&newHandler);
    
    // Remove the new test file
    std::filesystem::remove(newFilePath);
}

// Test metadata operations
TEST_F(CartHandlerTest, MetadataOperations) {
    CartHandler handler;
    ASSERT_EQ(cart_handler_open(&handler, testFilePath.c_str()), 0);
    
    Cart cart = {0};
    ASSERT_EQ(cart_handler_read_project(&handler, &cart), 0);
    
    // Test getting metadata
    char value[MAX_STR_LEN];
    EXPECT_EQ(cart_handler_get_meta_entry(&cart, "name", value), 0);
    EXPECT_STREQ(value, "test_project");
    
    EXPECT_EQ(cart_handler_get_meta_entry(&cart, "description", value), 0);
    EXPECT_STREQ(value, "Test Description");
    
    // Test setting metadata
    EXPECT_EQ(cart_handler_set_meta_entry(&cart, "name", "updated_name"), 0);
    EXPECT_EQ(cart_handler_set_meta_entry(&cart, "description", "Updated Description"), 0);
    
    // Verify the changes
    EXPECT_EQ(cart_handler_get_meta_entry(&cart, "name", value), 0);
    EXPECT_STREQ(value, "updated_name");
    
    EXPECT_EQ(cart_handler_get_meta_entry(&cart, "description", value), 0);
    EXPECT_STREQ(value, "Updated Description");
    
    // Test getting a non-existent entry
    EXPECT_NE(cart_handler_get_meta_entry(&cart, "nonexistent", value), 0);
    
    free_cart(&cart);
    cart_handler_close(&handler);
}

// Test feature manipulation through cart handler
TEST_F(CartHandlerTest, FeatureManipulation) {
    CartHandler handler;
    ASSERT_EQ(cart_handler_open(&handler, testFilePath.c_str()), 0);
    
    Cart cart = {0};
    ASSERT_EQ(cart_handler_read_project(&handler, &cart), 0);
    
    // Initialize the cart for features
    if (cart.features == NULL) {
        cart.features = (Feature**)malloc(MAX_FEATURES * sizeof(Feature*));
        for (int i = 0; i < MAX_FEATURES; i++) {
            cart.features[i] = NULL;
        }
    }
    
    // Add a feature
    Feature* feature1 = (Feature*)malloc(sizeof(Feature));
    memset(feature1, 0, sizeof(Feature));
    
    strncpy(feature1->id, "001", MAX_ID_LEN - 1);
    strncpy(feature1->name, "Feature 1", MAX_STR_LEN - 1);
    strncpy(feature1->description, "Description 1", MAX_STR_LEN - 1);
    feature1->status = STATUS_OPEN;
    feature1->priority = PRIORITY_HIGH;
    
    cart.features[cart.num_features++] = feature1;
    
    // Add another feature
    Feature* feature2 = (Feature*)malloc(sizeof(Feature));
    memset(feature2, 0, sizeof(Feature));
    
    strncpy(feature2->id, "002", MAX_ID_LEN - 1);
    strncpy(feature2->name, "Feature 2", MAX_STR_LEN - 1);
    strncpy(feature2->description, "Description 2", MAX_STR_LEN - 1);
    feature2->status = STATUS_IN_PROGRESS;
    feature2->priority = PRIORITY_MEDIUM;
    
    cart.features[cart.num_features++] = feature2;
    
    // Write the project back
    EXPECT_EQ(cart_handler_write_project(&handler, &cart), 0);
    EXPECT_EQ(cart_handler_save(&handler, testFilePath.c_str()), 0);
    
    // Read the project again to verify features
    free_cart(&cart);
    cart = {0};
    ASSERT_EQ(cart_handler_read_project(&handler, &cart), 0);
    
    ASSERT_EQ(cart.num_features, 2);
    
    // Verify Feature 1
    EXPECT_STREQ(cart.features[0]->id, "001");
    EXPECT_STREQ(cart.features[0]->name, "Feature 1");
    EXPECT_EQ(cart.features[0]->status, STATUS_OPEN);
    EXPECT_EQ(cart.features[0]->priority, PRIORITY_HIGH);
    
    // Verify Feature 2
    EXPECT_STREQ(cart.features[1]->id, "002");
    EXPECT_STREQ(cart.features[1]->name, "Feature 2");
    EXPECT_EQ(cart.features[1]->status, STATUS_IN_PROGRESS);
    EXPECT_EQ(cart.features[1]->priority, PRIORITY_MEDIUM);
    
    free_cart(&cart);
    cart_handler_close(&handler);
}