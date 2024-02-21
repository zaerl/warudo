cmake_minimum_required(VERSION 3.5)
project(Warudo
    VERSION 0.1.1
    DESCRIPTION "A webserver written in C"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Wno-unused-parameter")

# Config file
configure_file("config/config.h.in" "config/config.h")

## Main files
include_directories("src")

# Warudo library
file(GLOB SOURCES "src/*.c")
list(REMOVE_ITEM SOURCES "src/main.c")
add_library(warudo_lib STATIC ${SOURCES})
target_include_directories(warudo_lib PUBLIC "${PROJECT_BINARY_DIR}/config")

# SQLite3 library
file(GLOB SQLITE3_SOURCES "src/sqlite3/*.c")
add_library(warudo_sqlite3 STATIC ${SQLITE3_SOURCES})
target_compile_options(warudo_sqlite3
    PRIVATE
    -DSQLITE_DEFAULT_MEMSTATUS=0
    -DSQLITE_DQS=0
    -DSQLITE_LIKE_DOESNT_MATCH_BLOBS
    -DSQLITE_MAX_EXPR_DEPTH=0
    -DSQLITE_OMIT_AUTOINIT
    -DSQLITE_OMIT_DECLTYPE
    -DSQLITE_OMIT_DEPRECATED
    -DSQLITE_OMIT_PROGRESS_CALLBACK
    -DSQLITE_OMIT_SHARED_CACHE
    -DSQLITE_USE_ALLOCA
    -DSQLITE_USE_URI=1
)

add_executable(warudo "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")
target_link_libraries(warudo PRIVATE warudo_lib warudo_sqlite3)
set_target_properties(warudo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Create the test target
file(GLOB TEST_SOURCES "test/*.c")
add_executable(warudo-test ${TEST_SOURCES})
target_link_libraries(warudo-test PRIVATE warudo_lib warudo_sqlite3)
set_target_properties(warudo-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
target_include_directories(warudo-test PUBLIC "${PROJECT_BINARY_DIR}/config")

enable_testing()
