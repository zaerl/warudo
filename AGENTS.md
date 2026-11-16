# Warudo

Lightweight HTTPS web server written in C++20, in the spirit of nginx. The only external dependency is OpenSSL.

## Build

```sh
make                # configure + build (release preset)
make test           # build + run tests (test preset, defines WRD_TEST)
make ARGS="--filter=conf" test  # run filtered tests
make sanitize       # tests under AddressSanitizer + UndefinedBehaviorSanitizer
make start          # build + run with debug settings
make format         # apply .clang-format (make format_check only verifies)
make tidy           # run clang-tidy (.clang-tidy) on sources and tests
make clean          # remove build directory
```

Requires: CMake 3.21+, C++20 compiler (Clang or GCC), Python 3, OpenSSL 3.0+ development headers. `make format`/`make tidy` need LLVM's `clang-format`/`clang-tidy`.

The build is driven by `CMakePresets.json`; the Makefile is a thin wrapper (`cmake --preset <name>`, `cmake --build --preset <name>`, `ctest --preset <name>` work directly). Each preset builds in `build/<preset>/`:

| Preset | Purpose |
| --- | --- |
| `release` | Optimized server, no tests. Binary: `build/release/src/warudo` |
| `debug` | Debug server, no tests |
| `test` | Debug + test suite. Test binary: `build/test/tests/warudo-test` |
| `sanitize` | `test` with `-fsanitize=address,undefined` |
| `ci` | `test` with warnings as errors (used by `.github/workflows/ci.yml`) |

CMake targets: `warudo::options` (interface target carrying C++20, warnings, sanitizers and `WRD_TEST`), `warudo::core` (`warudo_lib`), `warudo` and `warudo-test`. Set flags on targets, never through `CMAKE_CXX_FLAGS`. Options: `WARUDO_BUILD_TESTS`, `WARUDO_WARNINGS_AS_ERRORS`, `WARUDO_SANITIZE`.

## Project structure

- `src/` - server core (C++ files + `warudo.h` main header), built as `warudo_lib`
- `src/json5.h`, `src/json5.cpp` - JSON5 reader (`wrd::json5`) used for the configuration file
- `tests/` - tests using [Attractor](https://github.com/zaerl/attractor) framework, mirrors `src/` file structure
- `tools/` - Python code generators (`generate-config.py`, `generate-tests.py`, `code-coverage.py`)
- `warudo.conf.default` - default config (JSON5 format), copy to `warudo.conf` to customize
- `TESTS.md` - test coverage tracking

## Code conventions

- C++20 standard (`.cpp` sources, no compiler extensions), compiled with `-Wall -Wextra -pedantic`. The code is still C-style: it was ported from C17 and is being modernized incrementally. New code is modern C++ in `namespace wrd` (see `src/json5.h`).
- Headers have C++ linkage (no `extern "C"`); use `static_cast` rather than implicit `void *` conversions, and plain `"..."` literals (not `u8"..."`, which is `char8_t` in C++20)
- All public symbols prefixed with `wrd_` (functions) or `WRD_` (macros/enums)
- Config values use `wrd_config_name` enum, env vars use `WRD_` prefix (e.g., `WRD_PORT`)
- Functions are declared in `warudo.h` (or module-specific headers like `mojibake.h`)
- Config and test boilerplate is code-generated — see below
- Formatting follows `.clang-format`. The existing code has not been mass-reformatted: format the files you touch as modules are modernized, and do not reformat generated regions by hand

## Auto-generated files

`make generate_config` (runs `python3 tools/generate-config.py h c conf server`) regenerates config-related sections in these files from `tools/configs.json`. **Do not add config entries manually** — add them to `tools/configs.json` and regenerate.

### `src/warudo.h` — two generated regions

1. **Enums and defines**: between `// Configurations.` and `// End Configurations.` — generates `wrd_config_name` enum, `wrd_log_level` enum, and all `#define WRD_DEFAULT_*` values.
2. **Struct fields**: between the second `// Configurations.` (inside the `warudo` struct) and `// End warudo configurations.` — generates `config_status[]`, and all config fields (`db_path`, `port`, `tls_enabled`, etc.).

Everything **outside** these markers (HTTP types, error codes, function declarations, and struct fields below `// End warudo configurations.` like `server_fd`, `tls_state`, `tls_ssl`, etc.) is **not generated** and is safe to edit manually.

### `src/conf.cpp` — four generated regions

All delimited by `// Configurations.` markers within their respective functions:

1. `wrd_config_init_defaults()` — default value assignments.
2. `wrd_load_config_env()` — environment variable loading.
3. `wrd_config_close()` — string field cleanup.
4. `wrd_load_config_file()` — loading from the parsed configuration file.

### `warudo.conf.default` — fully generated

The entire file content between `{` and `}` is regenerated.

### `src/server.cpp` — one generated region

Startup log lines between `// Configurations.` and `// Initialize TLS before forking workers`.

### `TESTS.md`

`tools/generate-tests.py` generates test coverage tracking.

## TLS / HTTPS

HTTPS support uses OpenSSL 3.0+ (`libssl`). Key source files:

- `src/tls.cpp` — TLS lifecycle: init (`wrd_init_tls`), per-connection handshake (`wrd_tls_handshake`), read/write wrappers, and cleanup (`wrd_tls_close`, `wrd_tls_finish_request`).
- TLS state is stored as opaque pointers in the `warudo` struct: `tls_state` (server-wide `SSL_CTX` with cert and key) and `tls_ssl` (per-connection `SSL`).
- `src/net.cpp` — socket creation, `select()`-based multiplexing for dual HTTP/HTTPS ports, and read/send functions that transparently dispatch to TLS or plain sockets based on whether `tls_ssl` is set.
- `src/worker.cpp` — worker loop performs TLS handshake after accept for HTTPS connections; plain HTTP connections are redirected to HTTPS when HSTS is active. `SIGPIPE` is ignored so broken connections return errors instead of killing workers.
- `src/server.cpp` — TLS is initialized before forking workers and cleaned up on server close.

Config keys: `tls_enabled`, `tls_cert_path`, `tls_key_path`, `tls_port`, `hsts_max_age`. Set these in `warudo.conf` or via `WRD_TLS_ENABLED`, `WRD_TLS_CERT_PATH`, `WRD_TLS_KEY_PATH`, `WRD_TLS_PORT`, `WRD_HSTS_MAX_AGE` env vars. When `tls_enabled` is on and `hsts_max_age > 0`, the server listens on both `port` (HTTP, sends 301 redirects) and `tls_port` (HTTPS).

## Routing

`src/route.cpp` dispatches each request by exact path match against the handlers registered with `wrd_route_add(config, "/path", handler)`; unmatched paths fall back to `wrd_route_static`, which serves files from `document_root`. Nothing registers a route yet: the table is the hook for future location handling. Handlers check the HTTP method and parse the raw `config->request_query_string` themselves.

## Configuration

Config file is JSON5 (`warudo.conf`). Every setting can be overridden via environment variable with `WRD_` prefix. See `warudo.conf.default` for all options.

The file is parsed by `wrd::json5::parse()` into a tree of `wrd::json5::value` (a `std::variant` of null, bool, 64-bit integer, double, string, array and object) and read by `wrd_load_config_file()` in `src/conf.cpp`:

- The document must be an object. A file with only white space and comments is valid and keeps every default; a malformed file makes `wrd_config_init` return `WRD_INVALID_CONFIG`.
- Unknown keys are ignored and the last duplicated key wins.
- A value of the wrong type keeps the default. Integer settings also accept booleans; string settings also accept integers and booleans (`worker_processes: 4`). `null` and `""` keep the default.
