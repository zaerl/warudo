set(SOURCES
    auth.c
    code.c
    conf.c
    db.c
    env.c
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
    version.c
)

add_library(warudo_lib STATIC ${SOURCES})

# warudo
add_executable(warudo main.c)
target_link_libraries(warudo PRIVATE warudo_lib warudo_sqlite)
