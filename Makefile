BUILD_DIR ?= build
BUILD_TYPE ?= Release
HCTREE_ARCHIVE ?= hctree.tar.gz
HCTREE_BUILD ?= hctree-build
HCTREE_URL ?= https://www.sqlite.org/hctree/tarball/hctree.tar.gz?r=hctree

all: configure build

configure:
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	@cmake --build $(BUILD_DIR)

rebuild: clean_build all

generate_config:
	python3 tools/generate-config.py h c conf server

generate_tests:
	python3 tools/generate-tests.py

code_coverage:
	python3 tools/code-coverage.py

start: configure build
	WRD_CORS="*" WRD_LOG_LEVEL=3 WRD_CONF_PATH=warudo.conf build/src/warudo

update_hctree:
	@mkdir -p $(HCTREE_BUILD)/build
	curl -L $(HCTREE_URL) | tar -xz -C $(HCTREE_BUILD)
	cd $(HCTREE_BUILD)/build && ../hctree/configure
	cd $(HCTREE_BUILD)/build && make sqlite3.c
	cp $(HCTREE_BUILD)/build/sqlite3.h src/sqlite3/sqlite3.h
	cp $(HCTREE_BUILD)/build/sqlite3.c src/sqlite3/sqlite3.c
	rm -rf $(HCTREE_BUILD)

docker:
	docker build --no-cache -t warudo:latest .

test: BUILD_TYPE = Test
test: configure build
	build/test/warudo-test $(ARGS)

clean_build:
	@cmake --build $(BUILD_DIR) --target clean

clean:
	rm -rf $(BUILD_DIR) $(HCTREE_BUILD)

.PHONY: all configure build clean rebuild
