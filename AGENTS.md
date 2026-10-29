# Warudo

Lightweight HTTPS web server written in C17 with no runtime dependencies, using SQLite (HCTree) for storage.

## Build

```sh
make                # configure + build (Release)
make test           # build + run tests (Test config with -DWRD_TEST)
make ARGS="--filter=conf" test  # run filtered tests
make start          # build + run with debug settings
make clean          # remove build directory
```

Requires: CMake 3.14+, C17 compiler (Clang), Python 3, OpenSSL 3.0+ development headers.

Build output goes to `build/`. Binary: `build/src/warudo`. Test binary: `build/tests/warudo-test`.

## Project structure

- `src/` - server core (C files + `warudo.h` main header), built as `warudo_lib`
- `src/app/` - example JSON entries application (`app.h`), built as `warudo_app` on top of the core
- `src/sqlite3/` - bundled SQLite/HCTree
- `tests/` - tests using [Attractor](https://github.com/zaerl/attractor) framework, mirrors `src/` file structure
- `tools/` - Python code generators (`generate-config.py`, `generate-tests.py`, `code-coverage.py`)
- `warudo.conf.default` - default config (JSON5 format), copy to `warudo.conf` to customize
- `TESTS.md` - test coverage tracking

## Code conventions

- C17 standard, compiled with `-Wall -Wextra -pedantic`
- All public symbols prefixed with `wrd_` (functions) or `WRD_` (macros/enums)
- Config values use `wrd_config_name` enum, env vars use `WRD_` prefix (e.g., `WRD_PORT`)
- Functions are declared in `warudo.h` (or module-specific headers like `mojibake.h`)
- Config and test boilerplate is code-generated — see below

## Auto-generated files

`make generate_config` (runs `python3 tools/generate-config.py h c conf server`) regenerates config-related sections in these files from `tools/configs.json`. **Do not add config entries manually** — add them to `tools/configs.json` and regenerate.

### `src/warudo.h` — two generated regions

1. **Enums and defines**: between `// Configurations.` and `// End Configurations.` — generates `wrd_config_name` enum, `wrd_log_level` enum, and all `#define WRD_DEFAULT_*` values.
2. **Struct fields**: between the second `// Configurations.` (inside the `warudo` struct) and `// End warudo configurations.` — generates `config_status[]`, and all config fields (`db_path`, `port`, `tls_enabled`, etc.).

Everything **outside** these markers (HTTP types, error codes, function declarations, and struct fields below `// End warudo configurations.` like `server_fd`, `tls_state`, `tls_ssl`, etc.) is **not generated** and is safe to edit manually.

### `src/conf.c` — four generated regions

All delimited by `// Configurations.` markers within their respective functions:

1. `wrd_config_init_defaults()` — default value assignments.
2. `wrd_load_config_env()` — environment variable loading.
3. `wrd_config_close()` — string field cleanup.
4. `wrd_config_init()` — database config loading (between `// Configurations.` and `wrd_load_config_env`).

### `warudo.conf.default` — fully generated

The entire file content between `{` and `}` is regenerated.

### `src/server.c` — one generated region

Startup log lines between `// Configurations.` and `ret = wrd_db_init(...)`.

### `TESTS.md`

`tools/generate-tests.py` generates test coverage tracking.

## TLS / HTTPS

HTTPS support uses OpenSSL 3.0+ (`libssl`). Key source files:

- `src/tls.c` — TLS lifecycle: init (`wrd_init_tls`), per-connection handshake (`wrd_tls_handshake`), read/write wrappers, and cleanup (`wrd_tls_close`, `wrd_tls_finish_request`).
- TLS state is stored as opaque pointers in the `warudo` struct: `tls_state` (server-wide `SSL_CTX` with cert and key) and `tls_ssl` (per-connection `SSL`).
- `src/net.c` — socket creation, `select()`-based multiplexing for dual HTTP/HTTPS ports, and read/send functions that transparently dispatch to TLS or plain sockets based on whether `tls_ssl` is set.
- `src/worker.c` — worker loop performs TLS handshake after accept for HTTPS connections; plain HTTP connections are redirected to HTTPS when HSTS is active. `SIGPIPE` is ignored so broken connections return errors instead of killing workers.
- `src/server.c` — TLS is initialized before forking workers and cleaned up on server close.

Config keys: `tls_enabled`, `tls_cert_path`, `tls_key_path`, `tls_port`, `hsts_max_age`. Set these in `warudo.conf` or via `WRD_TLS_ENABLED`, `WRD_TLS_CERT_PATH`, `WRD_TLS_KEY_PATH`, `WRD_TLS_PORT`, `WRD_HSTS_MAX_AGE` env vars. When `tls_enabled` is on and `hsts_max_age > 0`, the server listens on both `port` (HTTP, sends 301 redirects) and `tls_port` (HTTPS).

## Applications and routing

The core knows nothing about the application. An app is a `wrd_app` (`init`/`close` hooks) passed to `wrd_server_init(config, &app)`:

- `init` runs once after the configuration is loaded and before workers fork. It allocates its state in `config->app_data` and registers routes with `wrd_route_add(config, "/path", handler)`.
- `close` runs first in `wrd_server_close`, while the configuration is still loaded.
- `src/route.c` dispatches each request by exact path match; unmatched paths fall back to `wrd_route_static`. Handlers check the HTTP method and parse the raw `config->request_query_string` themselves.

`src/main.c` wires in the example app (`wrd_entries_app` from `src/app/app.c`). Tests for `src/app/*.c` live in `tests/app/`.

## Configuration

Config file is JSON5 (`warudo.conf`). Every setting can be overridden via environment variable with `WRD_` prefix. See `warudo.conf.default` for all options.
