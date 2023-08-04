CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -O2 -g
LDFLAGS = -L/opt/homebrew/opt/fastcgi/lib
LDLIBS = -lfcgi
FCGI_INCLUDE_PATH = /opt/homebrew/opt/fastcgi/include

SRC_DIR = src
TEST_SRC_DIR = test

BUILD_DIR = build
TEST_BUILD_DIR = test-build

BIN_DIR = bin
DBS = *.db *.db-shm *.db-wal

# Get all the src/ sub-directories
SUBDIRS := $(shell find $(SRC_DIR) -type d)

# Get all .c files from src/ directory
SRCS = $(shell find $(SRC_DIR) -type f -name "*.c")

# Generate corresponding .o file names
OBJS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRCS:.c=.o))

# Get all .c files from test/ directory
TEST_SRCS = $(shell find $(TEST_SRC_DIR) -type f -name "*.c" ! -name "main.c")

# Generate corresponding .o file names
TEST_OBJS = $(patsubst $(TEST_SRC_DIR)/%, $(TEST_BUILD_DIR)/%, $(TEST_SRCS:.c=.o))

# Subdirectories to create in the build directory
BUILD_SUBDIRS := $(foreach dir,$(SUBDIRS),$(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(dir)))

# Define the final executable name
TARGET = $(BIN_DIR)/warudo

# Define the test executable name
TEST_TARGET = $(BIN_DIR)/test

all: $(BUILD_DIR) $(BUILD_SUBDIRS) $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Create subdirectories in build directory
$(BUILD_SUBDIRS):
	mkdir -p $@

# Create test build directory if it doesn't exist
$(TEST_BUILD_DIR):
	mkdir -p $(TEST_BUILD_DIR)

# Compile all the source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -I$(FCGI_INCLUDE_PATH) $< -o $@

# Compile all the source files into object files
$(TEST_BUILD_DIR)/%.o: $(TEST_SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -I$(FCGI_INCLUDE_PATH) $< -o $@

# Link all the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -L$(FCGI_INCLUDE_PATH) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -L$(FCGI_INCLUDE_PATH) $(LDFLAGS) $(LDLIBS) $(filter-out build/main.o, $^) -o $@

test: $(BUILD_DIR) $(BUILD_SUBDIRS) $(TEST_BUILD_DIR) $(TEST_TARGET)
	$(TEST_TARGET)

start: $(BUILD_DIR) $(BUILD_SUBDIRS) $(TARGET)
	WARUDO_CORS="*" WARUDO_LOG_LEVEL=3 $(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TEST_BUILD_DIR) $(TARGET) ${DBS}

.PHONY: all clean

# stress-test:
# 	$(CC) $(CFLAGS) -o stress-test stress-test.c -L/opt/homebrew/opt/curl/lib -lcurl

#TARGET = warudo

#$(TARGET): warudo.o
#	$(CC) $(CFLAGS) -o $(TARGET) warudo.o -L$(FCGI_INCLUDE_PATH) $(LDFLAGS) $(LDLIBS)

#warudo.o: src/warudo.c
#	$(CC) $(CFLAGS) -c -I$(FCGI_INCLUDE_PATH) src/warudo.c

#sqlite3.o: src/sqlite3.c
#	$(CC) $(CFLAGS) -c src/sqlite3.c

#clean:
#	rm -f $(TARGET) *.o

#.PHONY: clean
