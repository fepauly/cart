cmake_minimum_required(VERSION 3.23)

project(CART C)

include_directories(include)

set(SOURCES
    src/commands.c
    src/main.c
    src/init.c
    src/pretty.c
    src/cart.c
    src/helper.c
)

add_executable(cart ${SOURCES})
set_target_properties(cart PROPERTIES
    C_STANDARD 11
)

target_compile_options(cart PRIVATE -Wall -Wextra -pedantic)