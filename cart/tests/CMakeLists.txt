include(GoogleTest)

# Common test properties
set(TEST_COMMON_DEPS 
    gtest_main
    ${LIBXML2_LIBRARIES}
)

# Define a macro to prevent multiple definitions of find_cart_file
add_compile_definitions(FIND_CART_FILE_DEFINED)

# Add filesystem library dependency for C++17
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    list(APPEND TEST_COMMON_DEPS stdc++fs)
endif()

# Cart Handler Test
add_executable(cart_handler_test cart_handler_test.cc)
target_link_libraries(cart_handler_test ${TEST_COMMON_DEPS} cart_lib)
target_include_directories(cart_handler_test PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)
gtest_discover_tests(cart_handler_test)

# ID Test
add_executable(id_test id_test.cc)
target_link_libraries(id_test ${TEST_COMMON_DEPS} cart_lib)
target_include_directories(id_test PRIVATE
    ${CMAKE_SOURCE_DIR}
)
gtest_discover_tests(id_test)

# Feature Test
add_executable(feature_test feature_test.cc)
target_link_libraries(feature_test ${TEST_COMMON_DEPS} cart_lib)
target_include_directories(feature_test PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)
gtest_discover_tests(feature_test)

# Deadline Test
add_executable(deadline_test deadline_test.cc)
target_link_libraries(deadline_test ${TEST_COMMON_DEPS} cart_lib)
target_include_directories(deadline_test PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)
gtest_discover_tests(deadline_test)

# Meta Test
add_executable(meta_test meta_test.cc)
target_link_libraries(meta_test ${TEST_COMMON_DEPS} cart_lib)
target_include_directories(meta_test PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)
gtest_discover_tests(meta_test)

# Add a target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS cart_handler_test id_test feature_test deadline_test meta_test
    COMMENT "Running all tests"
)