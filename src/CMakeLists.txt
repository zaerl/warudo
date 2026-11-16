set(SOURCES
    auth.cpp
    code.cpp
    conf.cpp
    data.cpp
    env.cpp
    fs.cpp
    hpack.cpp
    http.cpp
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
target_link_libraries(warudo_lib
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
)

# Example entries application
set(APP_SOURCES
    app/app.cpp
    app/db.cpp
)

add_library(warudo_app STATIC ${APP_SOURCES})
target_link_libraries(warudo_app PUBLIC warudo_lib)

# warudo
add_executable(warudo main.cpp)
target_link_libraries(warudo
    PRIVATE
        warudo_app
        warudo_sqlite
)
