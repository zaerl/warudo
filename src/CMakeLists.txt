set(SOURCES
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
    timing.cpp
    tls.cpp
    version.cpp
    worker.cpp
)

find_package(OpenSSL 3.0 REQUIRED)
add_library(warudo_lib STATIC ${SOURCES})
add_library(warudo::core ALIAS warudo_lib)
set_target_properties(warudo_lib PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(warudo_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(warudo_lib
    PUBLIC
        warudo::options
        OpenSSL::SSL
        OpenSSL::Crypto
)

# warudo
add_executable(warudo main.cpp)
set_target_properties(warudo PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(warudo PRIVATE warudo::core)
