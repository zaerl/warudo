# Warudo
Warudo is a lightweight HTTPS web server written in C17 with no run-time
dependencies and a client that processes JSON data.

## Installation
Warudo is a standalone application and doesn't rely on external libraries. The
`warudo` prebuilt binary is the best way to install Warudo. Get the binary,
copy it somewhere in the filesystem.

### Building the source code

Use the prebuilt binaries if you want to use this. But if you want to compile
Warudo you will need some tools.

1. [CMake](https://cmake.org).
2. [Clang](https://clang.llvm.org) or another C17-capable compiler.
3. [Python](https://www.python.org/).

After installing the required tools run:

```sh
make
```

## Usage
Warudo by default reads a file called `warudo.conf` in the same folder of the
binary. This behavior can be changed passing the `WRD_CONFIG_PATH` environment
variable.

Every config can be passed as environment variable. For example `socket_port`
can be passed as `WRD_SOCKET_PORT` variable.

```sh
WRD_CORS="*" WRD_LOG_LEVEL=3 ./warudo
```

Check `warudo.conf.default` for a list of default settings. You can copy that
file, rename it to `warudo.conf` and change that settings you want.

## CLI
```sh
make
cat file.log | ./tools/parse-log.sh
cat file.log | ./bin/warudo-cli
```

## Test
Warudo uses [Attractor](https://github.com/zaerl/attractor) for testing. Run:

```sh
make test
```

You can filter specific tests. e.g. `conf`:

```sh
make ARGS="--filter=conf" test
```

## FAQ

### Why?
Why not? And so on.

## Thanks
Warudo is built using the work of extraordinary individuals and teams.

1. CMake is distributed under the [OSI-approved BSD 3-clause License](https://gitlab.kitware.com/cmake/cmake/raw/master/Copyright.txt).
2. Python is distributed under the [PSFL](https://docs.python.org/3/license.html).
3. SQLite is in the [public domain](https://sqlite.org/copyright.html).
