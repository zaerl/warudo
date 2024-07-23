cmake_minimum_required(VERSION 3.14...3.30)

project(
    Warudo
    VERSION 0.1.0
    DESCRIPTION "Warudo"
    LANGUAGES C
)

if(APPLE)
    link_directories("/opt/homebrew/lib")
    include_directories("/opt/homebrew/include")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Wno-unused-parameter")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_TEST "-g -O0 -DWRD_TEST")

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Test" CACHE STRING "Configurations" FORCE)

# Warudo library
add_subdirectory(src)

# SQLite library
add_subdirectory(src/sqlite3)

# CLI
add_subdirectory(cli)

# Tests
add_subdirectory(test)
