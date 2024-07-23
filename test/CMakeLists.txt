set(TEST_SOURCES
    attractor/attractor.c
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
    queue.c
    route-app.c
    route-home.c
    server.c
    test.c
    test-utils.c
    timing.c
    tls.c
    version.c
)

add_executable(warudo-test ${TEST_SOURCES})
target_link_libraries(warudo-test PRIVATE warudo_lib warudo_sqlite warudo_data_lib)
