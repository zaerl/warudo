BUILD_DIR = build
BIN_DIR = bin

all: compile

compile: $(BUILD_DIR) $(BIN_DIR)
	cd $(BUILD_DIR) && cmake .. && make

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $@

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $@

start: compile
	WARUDO_CORS="*" WARUDO_LOG_LEVEL=3 $(BIN_DIR)/warudo

test: compile
	cd $(BIN_DIR) && ./warudo-test

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
