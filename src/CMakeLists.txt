set(SOURCES
    auth.c
    code.c
    conf.c
    data.c
    env.c
    fs.c
    hpack.c
    http.c
    log.c
    net.c
    output.c
    query.c
    route.c
    server.c
    timing.c
    tls.c
    version.c
    worker.c
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
    app/app.c
    app/db.c
)

add_library(warudo_app STATIC ${APP_SOURCES})
target_link_libraries(warudo_app PUBLIC warudo_lib)

# warudo
add_executable(warudo main.c)
target_link_libraries(warudo
    PRIVATE
        warudo_app
        warudo_sqlite
)
