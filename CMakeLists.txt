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

# Configurations
set(WRD_CONFIG_PATH "warudo.json")
set(WRD_DB_PATH "file:warudo.db")
set(WRD_LISTEN_BACKLOG 1024)
set(WRD_MAX_COLUMNS 64)
set(WRD_NET_BUFFER_SIZE 1)
set(WRD_NET_HEADERS_BUFFER_SIZE 4096)
set(WRD_NET_INPUT_BUFFER_SIZE 1)
set(WRD_SOCKET_PORT 6251)

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Wno-unused-parameter")

# Options
option(WARUDO_GENERATE_DEFAULT_CONFIG "Generate default configuration file" OFF)

# Config files
configure_file("config/config.h.in" "config/config.h")

if(WARUDO_GENERATE_DEFAULT_CONFIG)
    message(STATUS "Generating default configuration file")
    configure_file("config/warudo.conf.in" "${CMAKE_CURRENT_SOURCE_DIR}/warudo.conf.default")
endif()

## Main files
include_directories("src")

# re2c library
set(GENERATED_C_FILES)
file(GLOB_RECURSE RE_FILES "${SRC_DIR}/*.re")

foreach(RE_FILE ${RE_FILES})
    get_filename_component(RE_FILE_NAME ${RE_FILE} NAME)
    string(REPLACE ".re" "-generated.c" C_FILE_NAME ${RE_FILE_NAME})
    set(C_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/${C_FILE_NAME}")

    # Add the .c file to the list of generated files
    list(APPEND GENERATED_C_FILES ${C_FILE})

    # Add a custom command to compile the .re file to a .c file
    add_custom_command(
        OUTPUT ${C_FILE}
        COMMAND re2c ${RE_FILE} -o ${C_FILE}
        DEPENDS ${RE_FILE}
        COMMENT "Compiling ${RE_FILE} to ${C_FILE}"
        VERBATIM
    )
endforeach()

add_custom_target(re2c_compile ALL DEPENDS ${GENERATED_C_FILES})

# Warudo library
file(GLOB SOURCES "${SRC_DIR}/*.c")
list(APPEND SOURCES ${GENERATED_C_FILES})
list(REMOVE_ITEM SOURCES "${SRC_DIR}/main.c")
add_library(warudo_lib STATIC ${SOURCES})
target_include_directories(warudo_lib PUBLIC ${CONFIG_DIR})

# SQLite library
file(GLOB SQLITE3_SOURCES "${SRC_DIR}/sqlite3/*.c")
add_library(warudo_sqlite STATIC ${SQLITE3_SOURCES})
target_compile_options(warudo_sqlite
    PRIVATE
    -DSQLITE_DQS=0
    -DSQLITE_THREADSAFE=0
    -DSQLITE_DEFAULT_MEMSTATUS=0
    -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1
    -DSQLITE_LIKE_DOESNT_MATCH_BLOBS
    -DSQLITE_MAX_EXPR_DEPTH=0
    -DSQLITE_OMIT_DECLTYPE
    -DSQLITE_OMIT_DEPRECATED
    -DSQLITE_OMIT_PROGRESS_CALLBACK
    -DSQLITE_OMIT_SHARED_CACHE
    -DSQLITE_USE_ALLOCA
    -DSQLITE_OMIT_AUTOINIT
    -DSQLITE_STRICT_SUBTYPE=1
    -DSQLITE_USE_URI=1
)

# Data library
file(GLOB DATA_SOURCES "${CLI_DIR}/*.c")
list(REMOVE_ITEM DATA_SOURCES "${CLI_DIR}/main.c")
add_library(warudo_data_lib STATIC ${DATA_SOURCES})

# warudo
add_executable(warudo "${SRC_DIR}/main.c")
add_dependencies(warudo re2c_compile)
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
