BUILD_DIR ?= build
BUILD_TYPE ?= Release

all: configure build

configure: generate_config
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	@cmake --build $(BUILD_DIR)

rebuild: clean_build all

generate_config:
	php tools/generate-config.php h > src/conf.h
	php tools/generate-config.php c > src/conf.c
	php tools/generate-config.php conf > warudo.conf.default

generate_tests:
	php tools/generate-tests.php

code_coverage:
	php tools/code-coverage.php > TESTS.md

start: configure build
	WRD_CORS="*" WRD_LOG_LEVEL=3 build/src/warudo

test: BUILD_TYPE = Test
test: configure build
	build/test/warudo-test

clean_build:
	@cmake --build $(BUILD_DIR) --target clean

clean:
	rm -rf $(BUILD_DIR)
	rm src/conf.h src/conf.c warudo.conf.default

.PHONY: all configure build clean rebuild
