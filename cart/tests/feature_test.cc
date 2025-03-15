#include <gtest/gtest.h>
#include "cart_handler/cart.h"
#include "src/feature.h"
#include <cstring>
#include <fstream>
#include <filesystem>

// Test fixture for feature related tests
class FeatureTest : public ::testing::Test {
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
        
        // Set up find_cart_file to return our test file
        find_cart_file_func = [](char* filename, size_t size) {
            strncpy(filename, "test_cart.cart", size - 1);
            filename[size - 1] = '\0';
            return 0;
        };
    }

    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(testFilePath)) {
            std::filesystem::remove(testFilePath);
        }
        
        // Reset the mock function
        find_cart_file_func = nullptr;
    }

    std::string testFilePath;
    
    // Helper function to add a feature to the test cart file
    void AddFeatureToTestCart(const char* id, const char* name, const char* description, Status status, Priority priority) {
        CartHandler handler;
        ASSERT_EQ(cart_handler_open(&handler, testFilePath.c_str()), 0);
        
        Cart cart = {0};
        ASSERT_EQ(cart_handler_read_project(&handler, &cart), 0);
        
        // Create feature
        Feature* feature = (Feature*)malloc(sizeof(Feature));
        memset(feature, 0, sizeof(Feature));
        
        strncpy(feature->id, id, MAX_ID_LEN - 1);
        strncpy(feature->name, name, MAX_STR_LEN - 1);
        strncpy(feature->description, description, MAX_STR_LEN - 1);
        feature->status = status;
        feature->priority = priority;
        
        // Ensure features array is initialized
        if (cart.features == NULL) {
            cart.features = (Feature**)malloc(MAX_FEATURES * sizeof(Feature*));
            for (int i = 0; i < MAX_FEATURES; i++) {
                cart.features[i] = NULL;
            }
        }
        
        cart.features[cart.num_features++] = feature;
        
        ASSERT_EQ(cart_handler_write_project(&handler, &cart), 0);
        ASSERT_EQ(cart_handler_save(&handler, testFilePath.c_str()), 0);
        
        free_cart(&cart);
        cart_handler_close(&handler);
    }
};

// Test status and priority string conversions
TEST_F(FeatureTest, StatusAndPriorityConversion) {
    // Test status string conversion
    EXPECT_STREQ(status_to_string(STATUS_OPEN), "OPEN");
    EXPECT_STREQ(status_to_string(STATUS_IN_PROGRESS), "IN PROGRESS");
    EXPECT_STREQ(status_to_string(STATUS_DONE), "DONE");
    EXPECT_STREQ(status_to_string(STATUS_UNKNOWN), "UNKNOWN");
    
    // Test status enum conversion
    EXPECT_EQ(string_to_status("OPEN"), STATUS_OPEN);
    EXPECT_EQ(string_to_status("IN PROGRESS"), STATUS_IN_PROGRESS);
    EXPECT_EQ(string_to_status("DONE"), STATUS_DONE);
    EXPECT_EQ(string_to_status("UNKNOWN"), STATUS_UNKNOWN);
    EXPECT_EQ(string_to_status("INVALID"), STATUS_UNKNOWN);
    
    // Test priority string conversion
    EXPECT_STREQ(priority_to_string(PRIORITY_LOW), "LOW");
    EXPECT_STREQ(priority_to_string(PRIORITY_MEDIUM), "MEDIUM");
    EXPECT_STREQ(priority_to_string(PRIORITY_HIGH), "HIGH");
    EXPECT_STREQ(priority_to_string(PRIORITY_CRITICAL), "CRITICAL");
    EXPECT_STREQ(priority_to_string(PRIORITY_UNKNOWN), "UNKNOWN");
    
    // Test priority enum conversion
    EXPECT_EQ(string_to_priority("LOW"), PRIORITY_LOW);
    EXPECT_EQ(string_to_priority("MEDIUM"), PRIORITY_MEDIUM);
    EXPECT_EQ(string_to_priority("HIGH"), PRIORITY_HIGH);
    EXPECT_EQ(string_to_priority("CRITICAL"), PRIORITY_CRITICAL);
    EXPECT_EQ(string_to_priority("UNKNOWN"), PRIORITY_UNKNOWN);
    EXPECT_EQ(string_to_priority("INVALID"), STATUS_UNKNOWN);
}

// Test feature getting functionality
TEST_F(FeatureTest, GetFeature) {
    // Add a feature to test cart
    AddFeatureToTestCart("001", "Test Feature", "Feature Description", STATUS_OPEN, PRIORITY_HIGH);
    
    // Redirect stdout to capture output
    testing::internal::CaptureStdout();
    
    // Call the get feature command with our test ID
    const char* args[] = {"get", "001"};
    int result = cmd_feature_get(2, const_cast<char**>(args));
    
    // Get output
    std::string output = testing::internal::GetCapturedStdout();
    
    // Verify the function succeeded
    EXPECT_EQ(result, 0);
    
    // Verify output contains expected feature details
    EXPECT_TRUE(output.find("Feature #001: Test Feature") != std::string::npos);
    EXPECT_TRUE(output.find("Description: Feature Description") != std::string::npos);
    EXPECT_TRUE(output.find("Status: OPEN") != std::string::npos);
    EXPECT_TRUE(output.find("Priority: HIGH") != std::string::npos);
    
    // Test with non-existent feature ID
    testing::internal::CaptureStdout();
    const char* args2[] = {"get", "999"};
    result = cmd_feature_get(2, const_cast<char**>(args2));
    output = testing::internal::GetCapturedStdout();
    
    // Verify it fails properly
    EXPECT_NE(result, 0);
    EXPECT_TRUE(output.find("Feature with ID '999' not found") != std::string::npos);
}

// Test feature removal functionality
TEST_F(FeatureTest, RemoveFeature) {
    // Add two features to test cart
    AddFeatureToTestCart("001", "Feature 1", "Description 1", STATUS_OPEN, PRIORITY_HIGH);
    AddFeatureToTestCart("002", "Feature 2", "Description 2", STATUS_IN_PROGRESS, PRIORITY_MEDIUM);
    
    // Redirect stdout to capture output
    testing::internal::CaptureStdout();
    
    // Call the remove feature command for the first feature
    const char* args[] = {"remove", "001"};
    int result = cmd_feature_remove(2, const_cast<char**>(args));
    
    // Get output
    std::string output = testing::internal::GetCapturedStdout();
    
    // Verify the function succeeded
    EXPECT_EQ(result, 0);
    
    // Verify output
    EXPECT_TRUE(output.find("Removed feature 'Feature 1' with ID '001' successfully") != std::string::npos);
    
    // Now try to get the removed feature - should fail
    testing::internal::CaptureStdout();
    const char* get_args[] = {"get", "001"};
    result = cmd_feature_get(2, const_cast<char**>(get_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(result, 0);
    EXPECT_TRUE(output.find("Feature with ID '001' not found") != std::string::npos);
    
    // Get the second feature - should still exist
    testing::internal::CaptureStdout();
    const char* get_args2[] = {"get", "002"};
    result = cmd_feature_get(2, const_cast<char**>(get_args2));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("Feature #002: Feature 2") != std::string::npos);
}

// Test updating a feature
TEST_F(FeatureTest, UpdateFeature) {
    // Add a feature to test cart
    AddFeatureToTestCart("001", "Original Name", "Original Description", STATUS_OPEN, PRIORITY_MEDIUM);
    
    // Redirect stdout to capture output
    testing::internal::CaptureStdout();
    
    // Call the update feature command
    const char* args[] = {"update", "001", "-n", "Updated Name", "-d", "Updated Description", "-s", "done", "-p", "high"};
    int result = cmd_feature_update(10, const_cast<char**>(args));
    
    // Get output
    std::string output = testing::internal::GetCapturedStdout();
    
    // Verify the function succeeded
    EXPECT_EQ(result, 0);
    
    // Verify output
    EXPECT_TRUE(output.find("Updated feature 'Updated Name' with ID '001' successfully") != std::string::npos);
    
    // Now get the feature to verify updates
    testing::internal::CaptureStdout();
    const char* get_args[] = {"get", "001"};
    result = cmd_feature_get(2, const_cast<char**>(get_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("Feature #001: Updated Name") != std::string::npos);
    EXPECT_TRUE(output.find("Description: Updated Description") != std::string::npos);
    EXPECT_TRUE(output.find("Status: DONE") != std::string::npos);
    EXPECT_TRUE(output.find("Priority: HIGH") != std::string::npos);
}