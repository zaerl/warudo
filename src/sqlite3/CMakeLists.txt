set(SQLITE3_SOURCES
    sqlite3.c
)

add_library(warudo_sqlite STATIC ${SQLITE3_SOURCES})
target_compile_options(warudo_sqlite
    PRIVATE
    -DSQLITE_DQS=0
    -DSQLITE_THREADSAFE=0
    -DSQLITE_DEFAULT_MEMSTATUS=0
    -DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1
    -DSQLITE_LIKE_DOESNT_MATCH_BLOBS
    -DSQLITE_MAX_EXPR_DEPTH=0
    -DSQLITE_OMIT_DECLTYPE
    -DSQLITE_OMIT_DEPRECATED
    -DSQLITE_OMIT_PROGRESS_CALLBACK
    -DSQLITE_OMIT_SHARED_CACHE
    -DSQLITE_USE_ALLOCA
    -DSQLITE_OMIT_AUTOINIT
    -DSQLITE_STRICT_SUBTYPE=1
    -DSQLITE_USE_URI=1
    -Wno-macro-redefined # hctree
    -Wno-unused-function # hctree
    -Wno-unused-variable # hctree
    -Wno-sign-compare # hctree
    -Wno-missing-field-initializers # hctree
    -Wno-zero-length-array # hctree
)
