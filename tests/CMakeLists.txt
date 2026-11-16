set(TEST_SOURCES
    app/app.cpp
    app/db.cpp
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
    test.cpp
    test-utils.cpp
    timing.cpp
    tls.cpp
    version.cpp
    worker.cpp
)

add_executable(warudo-test ${TEST_SOURCES})
target_link_libraries(warudo-test PRIVATE warudo_app warudo_sqlite)
