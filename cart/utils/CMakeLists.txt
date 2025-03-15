add_library(utils_lib
    helper.c
    pretty.c
)

target_include_directories(utils_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)