add_library(cart_handler_lib
    cart.c
    id.c
)

target_include_directories(cart_handler_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/cart/utils
)

# Make sure id.h functions are accessible
target_compile_definitions(cart_handler_lib PUBLIC CART_HANDLER_EXPORTS)

target_link_libraries(cart_handler_lib ${LIBXML2_LIBRARIES} utils_lib)