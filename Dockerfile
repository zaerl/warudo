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

# musl's unistd.h header is located at the system headers root folder,
# /usr/include, and not under /usr/include/sys as in glibc.
RUN echo "#include <unistd.h>" > /usr/include/sys/unistd.h

RUN make test
RUN make clean
RUN make build

# FROM alpine:latest

# WORKDIR /app
# COPY --from=build /app/build/src/warudo /app/warudo.conf ./
# ENTRYPOINT ["./build/src/warudo"]
