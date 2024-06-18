cmake_minimum_required(VERSION 3.14...3.29)

project(
    Warudo
    VERSION 0.1.3
    DESCRIPTION "Warudo"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Wno-unused-parameter")

# Warudo library
add_subdirectory(src)

# SQLite library
add_subdirectory(src/sqlite3)

# CLI
add_subdirectory(cli)

# Tests
add_subdirectory(test)
