FROM alpine:latest

RUN apk update && \
    apk add --no-cache clang cmake lldb-dev py3-lldb python3 make

# Set Clang as the default C compiler
ENV CC=clang

WORKDIR /app
COPY CMakeLists.txt .
COPY Makefile .
COPY src ./src/
COPY test ./test/
COPY warudo.conf.default ./warudo.conf

RUN make build
# RUN build/test/warudo-test -v
# ENTRYPOINT ["./build/src/warudo"]
