set(SOURCES
    auth.c
    code.c
    conf.c
    data.c
    db.c
    env.c
    fs.c
    hpack.c
    http.c
    log.c
    net.c
    output.c
    query.c
    route-app.c
    route-home.c
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

# warudo
add_executable(warudo main.c)
target_link_libraries(warudo
    PRIVATE
        warudo_lib
        warudo_sqlite
)
