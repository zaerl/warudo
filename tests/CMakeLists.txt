set(TEST_SOURCES
    auth.cpp
    code.cpp
    conf.cpp
    data.cpp
    env.cpp
    fs.cpp
    hpack.cpp
    http.cpp
    json5.cpp
    log.cpp
    net.cpp
    output.cpp
    query.cpp
    route.cpp
    server.cpp
    test.cpp
    test-utils.cpp
    timing.cpp
    tls.cpp
    version.cpp
    worker.cpp
)

add_executable(warudo-test ${TEST_SOURCES})
set_target_properties(warudo-test PROPERTIES CXX_EXTENSIONS OFF)
target_compile_definitions(warudo-test PRIVATE WRD_TEST_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
target_link_libraries(warudo-test PRIVATE warudo::core)

# The tests create their temporary files in the working directory.
add_test(NAME warudo-test COMMAND warudo-test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
