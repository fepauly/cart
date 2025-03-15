#include <gtest/gtest.h>
#include "cart_handler/cart.h"
#include "src/deadline.h"
#include "utils/helper.h"
#include <cstring>
#include <fstream>
#include <filesystem>
#include <time.h>

class DeadlineTest : public ::testing::Test {
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
                << "    <deadline></deadline>\n"
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
    
    // Helper function to set the deadline in the test cart file
    void SetDeadlineInTestCart(const char* deadline) {
        CartHandler handler;
        ASSERT_EQ(cart_handler_open(&handler, testFilePath.c_str()), 0);
        
        Cart cart = {0};
        ASSERT_EQ(cart_handler_read_project(&handler, &cart), 0);
        
        ASSERT_EQ(cart_handler_set_meta_entry(&cart, "deadline", deadline), 0);
        
        ASSERT_EQ(cart_handler_write_project(&handler, &cart), 0);
        ASSERT_EQ(cart_handler_save(&handler, testFilePath.c_str()), 0);
        
        free_cart(&cart);
        cart_handler_close(&handler);
    }
};

// Test for helper functions related to dates
TEST_F(DeadlineTest, DateHelperFunctions) {
    // Test get_current_date_str
    char date_buffer[MAX_DATE_LEN];
    EXPECT_EQ(get_current_date_str(date_buffer, MAX_DATE_LEN), 0);
    
    // Verify the date is formatted correctly: MM/DD/YYYY
    ASSERT_EQ(strlen(date_buffer), 10);
    EXPECT_EQ(date_buffer[2], '/');
    EXPECT_EQ(date_buffer[5], '/');
    
    // Test get_current_date
    struct tm current_date = {0};
    EXPECT_EQ(get_current_date(&current_date), 0);
    
    // Test convert_str_to_date
    struct tm parsed_date = {0};
    EXPECT_EQ(convert_str_to_date("12/31/2023", &parsed_date), 0);
    EXPECT_EQ(parsed_date.tm_mday, 31);     // Day
    EXPECT_EQ(parsed_date.tm_mon, 11);      // Month (0-based)
    EXPECT_EQ(parsed_date.tm_year, 123);    // Year (1900-based)
    
    // Test days_between_dates
    struct tm date1 = {0}, date2 = {0};
    convert_str_to_date("01/01/2023", &date1);
    convert_str_to_date("01/02/2023", &date2);
    EXPECT_EQ(days_between_dates(&date1, &date2), 1);
    
    // Test with invalid dates
    EXPECT_NE(convert_str_to_date("13/01/2023", &parsed_date), 0);  // Invalid month
    EXPECT_NE(convert_str_to_date("12/32/2023", &parsed_date), 0);  // Invalid day
    EXPECT_NE(convert_str_to_date("12/00/2023", &parsed_date), 0);  // Invalid day
    EXPECT_NE(convert_str_to_date("00/01/2023", &parsed_date), 0);  // Invalid month
    EXPECT_NE(convert_str_to_date("12-01-2023", &parsed_date), 0);  // Wrong format
}

// Test setting and getting the deadline
TEST_F(DeadlineTest, SetAndGetDeadline) {
    // Test setting a valid deadline
    testing::internal::CaptureStdout();
    const char* set_args[] = {"set", "-d", "31", "-m", "12", "-y", "2023"};
    int result = cmd_deadline_set(7, const_cast<char**>(set_args));
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("Updated project deadline") != std::string::npos);
    
    // Test getting the deadline
    testing::internal::CaptureStdout();
    const char* get_args[] = {"get"};
    result = cmd_deadline_get(1, const_cast<char**>(get_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("12/31/2023") != std::string::npos);
}

// Test checking the deadline
TEST_F(DeadlineTest, CheckDeadline) {
    // Set up a deadline in the future
    SetDeadlineInTestCart("12/31/2024");
    
    // Test checking the deadline
    testing::internal::CaptureStdout();
    const char* check_args[] = {"check"};
    int result = cmd_deadline_check(1, const_cast<char**>(check_args));
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("You have") != std::string::npos);
    EXPECT_TRUE(output.find("days left") != std::string::npos);
    
    // Test with expired deadline
    SetDeadlineInTestCart("01/01/2020");
    
    testing::internal::CaptureStdout();
    result = cmd_deadline_check(1, const_cast<char**>(check_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(output.find("DEADLINE EXPIRED") != std::string::npos);
    
    // Test with no deadline set
    SetDeadlineInTestCart("");
    
    testing::internal::CaptureStdout();
    result = cmd_deadline_check(1, const_cast<char**>(check_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(result, 0);
    EXPECT_TRUE(output.find("No deadline set") != std::string::npos);
}

// Test error handling
TEST_F(DeadlineTest, ErrorHandling) {
    // Test with invalid date format
    testing::internal::CaptureStdout();
    const char* invalid_args[] = {"set", "-d", "32", "-m", "12", "-y", "2023"};
    int result = cmd_deadline_set(7, const_cast<char**>(invalid_args));
    std::string output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(result, 0);
    EXPECT_TRUE(output.find("Invalid date") != std::string::npos);
    
    // Test with date in the past
    testing::internal::CaptureStdout();
    const char* past_args[] = {"set", "-d", "1", "-m", "1", "-y", "2020"};
    result = cmd_deadline_set(7, const_cast<char**>(past_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(result, 0);
    EXPECT_TRUE(output.find("Deadline cannot be set before today's date") != std::string::npos);
    
    // Test with missing arguments
    testing::internal::CaptureStdout();
    const char* missing_args[] = {"set", "-d", "31"};
    result = cmd_deadline_set(3, const_cast<char**>(missing_args));
    output = testing::internal::GetCapturedStdout();
    
    EXPECT_NE(result, 0);
    EXPECT_TRUE(output.find("NOPE. There are values missing") != std::string::npos);
}