set(TEST_SOURCES
    app/app.c
    app/db.c
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
    test.c
    test-utils.c
    timing.c
    tls.c
    version.c
    worker.c
)

add_executable(warudo-test ${TEST_SOURCES})
target_link_libraries(warudo-test PRIVATE warudo_app warudo_sqlite)
