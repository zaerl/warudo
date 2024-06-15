BUILD_DIR = build
BIN_DIR = bin

all: compile

compile: $(BUILD_DIR) $(BIN_DIR) generate_config
	cd $(BUILD_DIR) && cmake .. && make

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $@

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $@

generate_config:
	php tools/generate-config.php h > src/conf.h
	php tools/generate-config.php c > src/conf.c
	php tools/generate-config.php conf > warudo.conf.default

# Generate warudo.conf.default file
generate_defaults: $(BUILD_DIR) $(BIN_DIR)
	cd $(BUILD_DIR) && cmake -DWARUDO_GENERATE_DEFAULT_CONFIG=ON ..

start: compile
	WRD_CORS="*" WRD_LOG_LEVEL=3 $(BIN_DIR)/warudo

test: compile
	cd $(BIN_DIR) && ./warudo-test

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	rm src/conf.h src/conf.c warudo.conf.default

.PHONY: all clean
