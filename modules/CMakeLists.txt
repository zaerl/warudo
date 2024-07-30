cmake_minimum_required(VERSION 3.14...3.30)

project(
    WarudoExampleModule
    VERSION 0.1.0
    DESCRIPTION "Warudo example module"
    LANGUAGES C
)

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

set(SOURCES
    main.c
)

# Add the library target
add_library(warudo_example SHARED ${SOURCES})

set_target_properties(warudo_example PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
