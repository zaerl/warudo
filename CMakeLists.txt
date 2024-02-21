cmake_minimum_required(VERSION 3.5)
project(Warudo
    VERSION 0.1.2
    DESCRIPTION "A webserver written in C11"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Various
set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CONFIG_DIR "${PROJECT_BINARY_DIR}/config")
set(SRC_DIR "src")
set(CLI_DIR "cli")

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Wno-unused-parameter")

# Config file
configure_file("config/config.h.in" "config/config.h")

## Main files
include_directories("src")

# Warudo library
file(GLOB SOURCES "${SRC_DIR}/*.c")
list(REMOVE_ITEM SOURCES "${SRC_DIR}/main.c")
add_library(warudo_lib STATIC ${SOURCES})
target_include_directories(warudo_lib PUBLIC ${CONFIG_DIR})

# SQLite library
file(GLOB SQLITE3_SOURCES "${SRC_DIR}/sqlite3/*.c")
add_library(warudo_sqlite STATIC ${SQLITE3_SOURCES})
target_compile_options(warudo_sqlite
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

# Data library
file(GLOB DATA_SOURCES "${CLI_DIR}/*.c")
list(REMOVE_ITEM DATA_SOURCES "${CLI_DIR}/main.c")
add_library(warudo_data_lib STATIC ${DATA_SOURCES})

# warudo
add_executable(warudo "${SRC_DIR}/main.c")
target_link_libraries(warudo PRIVATE warudo_lib warudo_sqlite)
set_target_properties(warudo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# warudo-test
file(GLOB TEST_SOURCES "test/*.c")
add_executable(warudo-test ${TEST_SOURCES})
target_link_libraries(warudo-test PRIVATE warudo_lib warudo_sqlite warudo_data_lib)
set_target_properties(warudo-test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
target_include_directories(warudo-test PUBLIC ${CONFIG_DIR})

# warudo-cli
add_executable(warudo-cli "${CLI_DIR}/main.c")

find_package(CURL REQUIRED)
target_link_libraries(warudo-cli PRIVATE warudo_data_lib ${CURL_LIBRARIES})
set_target_properties(warudo-cli PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
