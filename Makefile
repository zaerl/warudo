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
	WRD_CORS="*" WRD_LOG_LEVEL=3 build/src/warudo

test: compile
	build/test/warudo-test

clean:
	rm -rf build
	rm src/conf.h src/conf.c warudo.conf.default

.PHONY: all clean
