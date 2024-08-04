# syntax=docker/dockerfile:1
FROM alpine:latest AS build

RUN apk update && apk add --no-cache \
    clang \
    cmake \
    lldb-dev \
    make \
    py3-lldb \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# Set Clang as the default C compiler
ENV CC=clang

WORKDIR /app
COPY CMakeLists.txt .
COPY Makefile .
COPY src ./src/
COPY test ./test/
COPY warudo.conf.default ./warudo.conf

# RUN make build
RUN make test
RUN make clean
RUN make build

FROM alpine:latest

WORKDIR /app
COPY --from=build /app/build/src/warudo ./warudo
# ENTRYPOINT ["./build/src/warudo"]
