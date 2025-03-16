#include <gtest/gtest.h>
#include "cart_handler/cart.h"
#include "src/meta.h"
#include <cstring>
#include <fstream>
#include <filesystem>

// Define the testFilePath globally
static std::string testFilePath;

// Explicitly declare the external function pointer
extern "C" find_cart_file_fn find_cart_file_func;

class MetaTest : public ::testing::Test {
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
};

// Test for getting metadata entries
TEST_F(MetaTest, GetMetadataEntries) {
    testing::internal::CaptureStdout();
    
    // Test getting the name
    const char* name_args[] = {"get", "name"};
    int result = cmd_meta_get(2, const_cast<char**>(name_args));
    
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("name: test_project") != std::string::npos);
    
    // Test getting the description
    testing::internal::CaptureStdout();
    const char* desc_args[] = {"get", "description"};
    result = cmd_meta_get(2, const_cast<char**>(desc_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("description: Test Description") != std::string::npos);
    
    // Test getting the author
    testing::internal::CaptureStdout();
    const char* author_args[] = {"get", "author"};
    result = cmd_meta_get(2, const_cast<char**>(author_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("author: Test Author") != std::string::npos);
    
    // Test getting the created date
    testing::internal::CaptureStdout();
    const char* created_args[] = {"get", "created"};
    result = cmd_meta_get(2, const_cast<char**>(created_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("created: 01/01/2023") != std::string::npos);
    
    // Test getting the version
    testing::internal::CaptureStdout();
    const char* version_args[] = {"get", "version"};
    result = cmd_meta_get(2, const_cast<char**>(version_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("version: 1.0.0") != std::string::npos);
    
    // Test getting the deadline
    testing::internal::CaptureStdout();
    const char* deadline_args[] = {"get", "deadline"};
    result = cmd_meta_get(2, const_cast<char**>(deadline_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("deadline: 12/31/2023") != std::string::npos);
    
    // Test getting a non-existent entry
    testing::internal::CaptureStdout();
    const char* invalid_args[] = {"get", "nonexistent"};
    result = cmd_meta_get(2, const_cast<char**>(invalid_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(result, 0);
    EXPECT_TRUE(output.find("Entry not found") != std::string::npos);
}

// Test for setting metadata entries
TEST_F(MetaTest, SetMetadataEntries) {
    // Test setting the name
    testing::internal::CaptureStdout();
    const char* name_args[] = {"set", "name", "updated_project"};
    int result = cmd_meta_set(3, const_cast<char**>(name_args));
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("Updated metadata entry name to 'updated_project'") != std::string::npos);
    
    // Verify the name was updated
    testing::internal::CaptureStdout();
    const char* get_name_args[] = {"get", "name"};
    result = cmd_meta_get(2, const_cast<char**>(get_name_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("name: updated_project") != std::string::npos);
    
    // Test setting the description
    testing::internal::CaptureStdout();
    const char* desc_args[] = {"set", "description", "Updated Description"};
    result = cmd_meta_set(3, const_cast<char**>(desc_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("Updated metadata entry description to 'Updated Description'") != std::string::npos);
    
    // Test setting the author
    testing::internal::CaptureStdout();
    const char* author_args[] = {"set", "author", "Updated Author"};
    result = cmd_meta_set(3, const_cast<char**>(author_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("Updated metadata entry author to 'Updated Author'") != std::string::npos);
    
    // Test setting the version
    testing::internal::CaptureStdout();
    const char* version_args[] = {"set", "version", "2.0.0"};
    result = cmd_meta_set(3, const_cast<char**>(version_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("Updated metadata entry version to '2.0.0'") != std::string::npos);
    
    // Test that we can't set the created date (should be protected)
    testing::internal::CaptureStdout();
    const char* created_args[] = {"set", "created", "02/02/2023"};
    result = cmd_meta_set(3, const_cast<char**>(created_args));
    output = testing::internal::GetCapturedStdout();
    
    // This should be rejected with a message but not error out
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("The entry 'created' cannot be updated") != std::string::npos);
    
    // Test that deadline is redirected to the deadline command
    testing::internal::CaptureStdout();
    const char* deadline_args[] = {"set", "deadline", "12/31/2024"};
    result = cmd_meta_set(3, const_cast<char**>(deadline_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("To set the deadline use: cart deadline set") != std::string::npos);
}

// Test for listing all metadata entries
TEST_F(MetaTest, ListMetadataEntries) {
    testing::internal::CaptureStdout();
    
    const char* list_args[] = {"list"};
    int result = cmd_meta_list(1, const_cast<char**>(list_args));
    
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    
    // Check that all metadata entries are listed
    EXPECT_TRUE(output.find("Project Metadata") != std::string::npos);
    EXPECT_TRUE(output.find("Name") != std::string::npos);
    EXPECT_TRUE(output.find("test_project") != std::string::npos);
    EXPECT_TRUE(output.find("Description") != std::string::npos);
    EXPECT_TRUE(output.find("Test Description") != std::string::npos);
    EXPECT_TRUE(output.find("Author") != std::string::npos);
    EXPECT_TRUE(output.find("Test Author") != std::string::npos);
    EXPECT_TRUE(output.find("Created") != std::string::npos);
    EXPECT_TRUE(output.find("01/01/2023") != std::string::npos);
    EXPECT_TRUE(output.find("Version") != std::string::npos);
    EXPECT_TRUE(output.find("1.0.0") != std::string::npos);
    EXPECT_TRUE(output.find("Deadline") != std::string::npos);
    EXPECT_TRUE(output.find("12/31/2023") != std::string::npos);
}