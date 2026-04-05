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

Requires: CMake 3.14+, C17 compiler (Clang), Python 3.

Build output goes to `build/`. Binary: `build/src/warudo`. Test binary: `build/test/warudo-test`.

## Project structure

- `src/` - server source (C files + `warudo.h` main header)
- `src/sqlite3/` - bundled SQLite/HCTree
- `test/` - tests using [Attractor](https://github.com/zaerl/attractor) framework, mirrors `src/` file structure
- `tools/` - Python code generators (`generate-config.py`, `generate-tests.py`, `code-coverage.py`)
- `warudo.conf.default` - default config (JSON5 format), copy to `warudo.conf` to customize
- `TESTS.md` - test coverage tracking

## Code conventions

- C17 standard, compiled with `-Wall -Wextra -pedantic`
- All public symbols prefixed with `wrd_` (functions) or `WRD_` (macros/enums)
- Config values use `wrd_config_name` enum, env vars use `WRD_` prefix (e.g., `WRD_SOCKET_PORT`)
- Functions are declared in `warudo.h` (or module-specific headers like `mojibake.h`)
- Config and test boilerplate is code-generated — see below

## Auto-generated files

`make generate_config` (runs `python3 tools/generate-config.py h c conf server`) regenerates config-related sections in these files from `tools/configs.json`:

- `src/warudo.h` — enum `wrd_config_name`, `#define WRD_DEFAULT_*` values, and struct fields between `// Configurations.` and `// End Configurations.` (or `// Network` for the struct). **Do not add config entries manually** — add them to `tools/configs.json` and regenerate.
- `src/conf.c` — `wrd_config_init_defaults()`, `wrd_load_config_env()`, `wrd_config_close()`, and db config loading sections between `// Configurations.` markers.
- `warudo.conf.default` — full default config file.
- `src/server.c` — startup log lines between `// Configurations.` and `ret = wrd_db_init(...)`.

Non-config code in `src/warudo.h` (HTTP types, error codes, function declarations) is **not generated** and is safe to edit manually. Keep manual additions outside the `// Configurations.` / `// End Configurations.` markers.

`tools/generate-tests.py` generates `TESTS.md` test coverage tracking.

## TLS / HTTPS

HTTPS support uses mbedTLS (4.x) with PSA Crypto for RNG. Key source files:

- `src/tls.c` — TLS lifecycle: init (`wrd_init_tls`), per-connection handshake (`wrd_tls_handshake`), read/write wrappers, and cleanup (`wrd_tls_close`, `wrd_tls_finish_request`).
- TLS state is stored as opaque pointers in the `warudo` struct: `tls_state` (server-wide config, certs, keys) and `tls_ssl` (per-connection SSL context).
- `src/net.c` — read/send functions transparently dispatch to TLS or plain sockets based on whether `tls_ssl` is set.
- `src/worker.c` — worker loop performs TLS handshake after accept when TLS is enabled; `SIGPIPE` is ignored so broken connections return errors instead of killing workers.
- `src/server.c` — TLS is initialized before forking workers and cleaned up on server close.

Config keys: `tls_enabled`, `tls_cert_path`, `tls_key_path`. Set these in `warudo.conf` or via `WRD_TLS_ENABLED`, `WRD_TLS_CERT_PATH`, `WRD_TLS_KEY_PATH` env vars.

## Configuration

Config file is JSON5 (`warudo.conf`). Every setting can be overridden via environment variable with `WRD_` prefix. See `warudo.conf.default` for all options.
