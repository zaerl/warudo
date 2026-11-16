cmake_minimum_required(VERSION 3.14...3.31)

project(
    Warudo
    VERSION 0.1.0
    DESCRIPTION "Warudo"
    LANGUAGES C CXX
)

if(APPLE)
    link_directories("/opt/homebrew/lib")
    include_directories("/opt/homebrew/include")

    # Homebrew OpenSSL is keg-only, point FindOpenSSL at it.
    if(NOT OPENSSL_ROOT_DIR AND EXISTS "/opt/homebrew/opt/openssl")
        set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl")
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Warudo is C++20. C is only used for the bundled SQLite amalgamation.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Wno-unused-parameter")
# -Wno-write-strings: configuration strings are still `char *` fields defaulting to
# string literals. Remove once they become std::string.
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-unused-parameter -Wno-write-strings")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_TEST "-g -O0 -DWRD_TEST")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_TEST "-g -O0 -DWRD_TEST")

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Test" CACHE STRING "Configurations" FORCE)

# Warudo library
add_subdirectory(src)

# SQLite library
add_subdirectory(src/sqlite3)

# Tests
add_subdirectory(tests)
