cmake_minimum_required(VERSION 3.14...3.29)

project(
    Warudo
    VERSION 0.1.2
    DESCRIPTION "Warudo"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Various
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(CLI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cli")

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Wno-unused-parameter")

## Main files
#include_directories("src")

# Warudo library
add_subdirectory(src)

# Sqlite library
add_subdirectory(src/sqlite3)

# CLI
add_subdirectory(cli)

# warudo
add_executable(warudo "${SRC_DIR}/main.c")
target_link_libraries(warudo PRIVATE warudo_lib warudo_sqlite)

# warudo-cli
add_executable(warudo-cli "${CLI_DIR}/main.c")
find_package(CURL REQUIRED)
target_link_libraries(warudo-cli PRIVATE warudo_data_lib ${CURL_LIBRARIES})

# warudo-test
file(GLOB_RECURSE TEST_SOURCES "test/*.c")
add_executable(warudo-test ${TEST_SOURCES})
target_link_libraries(warudo-test PRIVATE warudo_lib warudo_sqlite warudo_data_lib)
