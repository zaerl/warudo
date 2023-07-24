# Warudo
Warudo is a lightweight server and client that process JSON.

## Installation
Warudo is a FCGI application so it needs a webserver that interface with
external programs that support that interface.

## Nginx

### Macos

/opt/homebrew/opt/nginx/bin/nginx -g "daemon off;"
make
make && WARUDO_CORS="*" bin/warudo
