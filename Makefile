BUILD_DIR ?= build
PRESET ?= release

# Homebrew LLVM is keg-only: fall back to it when the tools are not in the PATH.
LLVM_DIRS = $(LLVM_BIN) /opt/homebrew/opt/llvm/bin /usr/local/opt/llvm/bin
find_llvm_tool = $(firstword $(shell command -v $(1)) $(wildcard $(addsuffix /$(1),$(LLVM_DIRS))) $(1))
CLANG_FORMAT ?= $(call find_llvm_tool,clang-format)
CLANG_TIDY ?= $(call find_llvm_tool,clang-tidy)
CXX_FILES = $(wildcard src/*.cpp src/*.h tests/*.cpp) tests/test.h

all: build

# Presets are defined in CMakePresets.json. Each one builds in $(BUILD_DIR)/<preset>.
configure:
	@cmake --preset $(PRESET)

build: configure
	@cmake --build --preset $(PRESET)

rebuild: clean_build all

generate_config:
	python3 tools/generate-config.py h c conf server

generate_tests:
	python3 tools/generate-tests.py

code_coverage:
	python3 tools/code-coverage.py

start: build
	WRD_CORS="*" WRD_LOG_LEVEL=3 WRD_CONF_PATH=warudo.conf $(BUILD_DIR)/$(PRESET)/src/warudo

docker:
	docker build --no-cache -t warudo:latest .

test: PRESET = test
test: build
	$(BUILD_DIR)/$(PRESET)/tests/warudo-test $(ARGS)

# Tests with AddressSanitizer and UndefinedBehaviorSanitizer.
sanitize: PRESET = sanitize
sanitize: build
	$(BUILD_DIR)/$(PRESET)/tests/warudo-test $(ARGS)

format:
	$(CLANG_FORMAT) -i $(CXX_FILES)

format_check:
	$(CLANG_FORMAT) --dry-run --Werror $(CXX_FILES)

# Uses the compilation database of the test preset.
tidy: PRESET = test
tidy: configure
	$(CLANG_TIDY) -p $(BUILD_DIR)/$(PRESET) $(filter %.cpp,$(CXX_FILES))

clean_build:
	@cmake --build --preset $(PRESET) --target clean

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all configure build rebuild start test sanitize format format_check tidy clean_build clean \
	generate_config generate_tests code_coverage docker
