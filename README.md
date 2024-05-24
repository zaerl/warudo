# Warudo
Warudo is a lightweight web server written in C11 with no run-time dependencies
and a client that processes JSON data.

## Installation
Warudo is a standalone application and doesn't rely on external libraries. But
you need some tools to compile it.

1. [CMake](https://cmake.org/).
2. [Clang](https://clang.llvm.org/) or another C11-capable compiler.
3. [re2c](https://re2c.org/).

After installing the required tools run:

```sh
make
```

## Test
Warudo uses [Attractor](https://github.com/zaerl/attractor) for testing. Run:

```sh
make test
```

## Usage
Warudo by default reads a file called `warudo.conf` in the same folder of the
binary. This behavior can be changed passing the `WRD_CONFIG_PATH` environment
variable. Every config can be passed as environment variable. Check
`warudo.conf.default` for a list of default settings.

Example:

```sh
WRD_CORS="*" WRD_LOG_LEVEL=3 ./bin/warudo
```

## CLI
```sh
make
cat file.log | ./tools/parse-log.sh
cat file.log | ./bin/warudo-cli
```

## FAQ

### Why?
Why not? And so on.

## Thanks
Warudo is built using the work of extraordinary individuals and teams.

1. CMake is distributed under the [OSI-approved BSD 3-clause License](https://gitlab.kitware.com/cmake/cmake/raw/master/Copyright.txt).
2. SQLite is in the [public domain](https://sqlite.org/copyright.html).
3. re2c is in the [Public Domain](https://re2c.org/#license).
