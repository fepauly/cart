add_executable(cart main.c)

target_link_libraries(cart 
    cart_lib
    ${LIBXML2_LIBRARIES}
)

target_include_directories(cart PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)