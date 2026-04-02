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
- Config and test boilerplate is code-generated via `tools/generate-config.py` and `tools/generate-tests.py`

## Configuration

Config file is JSON5 (`warudo.conf`). Every setting can be overridden via environment variable with `WRD_` prefix. See `warudo.conf.default` for all options.
