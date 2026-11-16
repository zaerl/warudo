cmake_minimum_required(VERSION 3.21...3.31)

project(
    Warudo
    VERSION 0.1.0
    DESCRIPTION "Warudo"
    LANGUAGES CXX
)

# Options.
option(WARUDO_BUILD_TESTS "Build the Warudo test suite" ${PROJECT_IS_TOP_LEVEL})
option(WARUDO_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
set(WARUDO_SANITIZE "" CACHE STRING "Sanitizers to enable, e.g. \"address,undefined\"")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Used by clang-tidy and editors.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Homebrew OpenSSL is keg-only, point FindOpenSSL at it.
if(APPLE AND NOT OPENSSL_ROOT_DIR AND EXISTS "/opt/homebrew/opt/openssl")
    set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl")
endif()

# Build settings shared by every Warudo target.
add_library(warudo_options INTERFACE)
add_library(warudo::options ALIAS warudo_options)
target_compile_features(warudo_options INTERFACE cxx_std_20)
target_compile_options(warudo_options
    INTERFACE
        -Wall
        -Wextra
        -pedantic
        -Wno-unused-parameter
        # Configuration strings are still `char *` fields defaulting to string literals.
        # Remove once they become std::string.
        -Wno-write-strings
        $<$<BOOL:${WARUDO_WARNINGS_AS_ERRORS}>:-Werror>
)

if(WARUDO_WARNINGS_AS_ERRORS)
    # hpack.cpp still uses sprintf(), which the macOS SDK marks as deprecated.
    target_compile_options(warudo_options INTERFACE -Wno-error=deprecated-declarations)
endif()

if(WARUDO_SANITIZE)
    target_compile_options(warudo_options
        INTERFACE -fsanitize=${WARUDO_SANITIZE} -fno-omit-frame-pointer)
    target_link_options(warudo_options INTERFACE -fsanitize=${WARUDO_SANITIZE})
endif()

if(WARUDO_BUILD_TESTS)
    target_compile_definitions(warudo_options INTERFACE WRD_TEST)
endif()

# Warudo library and server
add_subdirectory(src)

# Tests
if(WARUDO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
