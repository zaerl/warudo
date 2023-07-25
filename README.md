# Warudo
Warudo is a lightweight server and client that process JSON data written in C11.

## Installation
Warudo is a FCGI application so it needs a webserver that interface with
external programs that support that interface.

## Nginx

### Macos

/opt/homebrew/opt/nginx/bin/nginx -g "daemon off;"
make
make && WARUDO_CORS="*" bin/warudo

### Thanks
Warudo is built using work of amazing individuals.

1. SQLite (Public Domain): https://sqlite.org/
2. FastCGI (Open Market License): https://fastcgi-archives.github.io
3. Vue (MIT License): https://vuejs.org
4. Apache ECharts (Apache License 2.0): https://echarts.apache.org
