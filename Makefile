all: compile

compile: generate_config
	cmake -S . -B build
	cmake --build build

generate_config:
	php tools/generate-config.php h > src/conf.h
	php tools/generate-config.php c > src/conf.c
	php tools/generate-config.php conf > warudo.conf.default

code_coverage:
	php tools/code-coverage.php > TESTS.md

start: compile
	WRD_CORS="*" WRD_LOG_LEVEL=3 $(BIN_DIR)/warudo

test: compile
	cd $(BIN_DIR) && ./warudo-test

clean:
	rm -rf build bin
	rm src/conf.h src/conf.c warudo.conf.default

.PHONY: all clean
