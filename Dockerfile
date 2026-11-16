# syntax=docker/dockerfile:1
FROM alpine:latest AS build

RUN apk update && apk add --no-cache \
    build-base \
    clang \
    cmake \
    lldb-dev \
    make \
    openssl-dev \
    openssl-libs-static \
    py3-lldb \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# Set Clang as the default C++ compiler
ENV CXX=clang++

WORKDIR /app
COPY CMakeLists.txt CMakePresets.json ./
COPY Makefile .
COPY src ./src/
COPY tests ./tests/
COPY warudo.conf.default ./warudo.conf

# musl's unistd.h header is located at the system headers root folder,
# /usr/include, and not under /usr/include/sys as in glibc.
RUN echo "#include <unistd.h>" > /usr/include/sys/unistd.h

RUN make test
RUN make clean
RUN make build

# FROM alpine:latest

# WORKDIR /app
# COPY --from=build /app/build/release/src/warudo /app/warudo.conf ./
# ENTRYPOINT ["./warudo"]
