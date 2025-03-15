add_library(cart_src
    commands.c
    deadline.c
    feature.c
    init.c
    meta.c
    help.c
)

target_include_directories(cart_src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cart_src cart_handler_lib utils_lib ${LIBXML2_LIBRARIES})