cmake_minimum_required(VERSION 3.5)
project(Warudo
    VERSION 0.1.1
    DESCRIPTION "A webserver written in C"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
set(SQLITE_CFLAGS "-DSQLITE_USE_URI=1 -DSQLITE_DQS=0 -DSQLITE_DEFAULT_MEMSTATUS=0 -DSQLITE_LIKE_DOESNT_MATCH_BLOBS")
set(SQLITE_CFLAGS_2 "-DSQLITE_MAX_EXPR_DEPTH=0 -DSQLITE_OMIT_DECLTYPE -DSQLITE_OMIT_DEPRECATED -DSQLITE_OMIT_PROGRESS_CALLBACK")
set(SQLITE_CFLAGS_3 "-DSQLITE_OMIT_SHARED_CACHE -DSQLITE_USE_ALLOCA -DSQLITE_OMIT_AUTOINIT")
set(CFLAGS "-Wall -Wextra -pedantic -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SQLITE_CFLAGS} ${SQLITE_CFLAGS_2} ${SQLITE_CFLAGS_3} ${CFLAGS}")

# Executable
include_directories("src")
file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(warudo ${SOURCES})
set_target_properties(warudo PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Config file
configure_file("config/config.h.in" "config/config.h")
target_include_directories(warudo PUBLIC "${PROJECT_BINARY_DIR}/config")

# Create the test target
file(GLOB_RECURSE TEST_SOURCES "test/*.c")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")
add_executable(warudo-test ${SOURCES} ${TEST_SOURCES})
set_target_properties(warudo-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

# Config file
target_include_directories(warudo-test PUBLIC "${PROJECT_BINARY_DIR}/config")

enable_testing()
