set(DATA_SOURCES
    data.c
)

add_library(warudo_data_lib STATIC ${DATA_SOURCES})

add_executable(warudo-cli main.c)
find_package(CURL REQUIRED)
target_link_libraries(warudo-cli PRIVATE warudo_data_lib ${CURL_LIBRARIES})
