add_subdirectory(utils)
add_subdirectory(cart_handler)
add_subdirectory(src)

# Define the cart_lib library that combines all components
add_library(cart_lib INTERFACE)
target_link_libraries(cart_lib INTERFACE cart_handler_lib cart_src utils_lib)

add_subdirectory(tests)