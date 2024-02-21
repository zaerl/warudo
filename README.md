# Warudo
Warudo is a lightweight web server written in C11 with no run-time dependencies
and a client that processes JSON data.

## Installation
Warudo is a standalone application.

```
make
WARUDO_CORS="*" WARUDO_LOG_LEVEL=3 ./bin/warudo
```

## CLI
```
make
cat file.log | ./tools/parse-log.sh
cat file.log | ./bin/warudo-cli
```

## HC-Tree
```
make hctree.fossil
make hctree-amalgamation
```

## Compile
Using the provided tarball is enough. If you want to compile the project you
need `cmake` and a C11-capable compiler.

## Test
`make test`

## FAQ

### Why? There are (insert here a long list of webservers)
Why not?

### Why in C11?
Why not?

### Why not using CGI, FCGI, etc.?
Why not?

And so on.

## Thanks
Warudo is built using the work of extraordinary individuals.

1. SQLite (Public Domain): https://sqlite.org/
2. FastCGI (Open Market License): https://fastcgi-archives.github.io
3. Vue (MIT License): https://vuejs.org
4. Apache ECharts (Apache License 2.0): https://echarts.apache.org
