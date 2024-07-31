BUILD_DIR ?= build
BUILD_TYPE ?= Release

all: configure build

configure: generate_config
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	@cmake --build $(BUILD_DIR)

rebuild: clean_build all

generate_config:
	python3 tools/generate-config.py h c conf server

generate_tests:
	python3 tools/generate-tests.py

code_coverage:
	python3 tools/code-coverage.py > TESTS.md

start: configure build
	WRD_CORS="*" WRD_LOG_LEVEL=3 WRD_CONF_PATH=warudo.conf build/src/warudo

test: BUILD_TYPE = Test
test: configure build
	build/test/warudo-test $(ARGS)

clean_build:
	@cmake --build $(BUILD_DIR) --target clean

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all configure build clean rebuild
