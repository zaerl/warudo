DEBUG ?= 0
CC = clang
CFLAGS = -Wall -Wextra -pedantic -std=c11 -DSQLITE_USE_URI=1
TEST_CFLAGS = -Wno-int-conversion -Wno-string-compare -Wno-format -Wno-pointer-to-int-cast
TEST_CFLAGS_2 = -Wno-void-pointer-to-int-cast -Wno-incompatible-pointer-types-discards-qualifiers
TEST_CFLAGS_3 = -Wno-unused-parameter -Wno-gnu-zero-variadic-macro-arguments
HCTREE_CFLAGS = -Wno-unused-parameter
LDFLAGS = -L/opt/homebrew/opt/fastcgi/lib
LDLIBS = -lfcgi
FCGI_INCLUDE_PATH = /opt/homebrew/opt/fastcgi/include

SRC_DIR = src
TEST_SRC_DIR = test
TOOLS_DIR = tools

BUILD_DIR = build
TEST_BUILD_DIR = test-build

# HC-tree
HC_TREE_BUILD = hctree-build
HC_TREE_SRC = hctree-src
HC_TREE_REPO = hctree.fossil
HC_TREE_AMALGAMATION_DIR = $(SRC_DIR)/sqlite3

BIN_DIR = bin
DBS = *.db *.db-shm *.db-wal

FORMATTER = clang-format

# Debug Settings
ifeq ($(debug), 1)
	CC := $(CC) -g -O0
else
	CC := $(CC) -Oz
endif

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

STRESS_TEST_TARGET = $(BIN_DIR)/stress-test

all: $(BUILD_DIR) $(BUILD_SUBDIRS) $(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $@

# Create subdirectories in build directory
$(BUILD_SUBDIRS):
	mkdir -p $@

# Create test build directory if it doesn't exist
$(TEST_BUILD_DIR):
	mkdir -p $@

# Compile all the source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -I$(FCGI_INCLUDE_PATH) $< -o $@

# Compile all the source files into object files
$(TEST_BUILD_DIR)/%.o: $(TEST_SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $(TEST_CFLAGS_2) $(TEST_CFLAGS_3) -c -I$(FCGI_INCLUDE_PATH) $< -o $@

# Link all the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -L$(FCGI_INCLUDE_PATH) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(TEST_TARGET): $(OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -L$(FCGI_INCLUDE_PATH) $(LDFLAGS) $(LDLIBS) $(filter-out build/main.o, $^) -o $@

$(STRESS_TEST_TARGET): stress-test.c
	$(CC) $(CFLAGS) -lcurl -o $@ stress-test.c

test: $(BUILD_DIR) $(BUILD_SUBDIRS) $(TEST_BUILD_DIR) $(TEST_TARGET)
	$(TEST_TARGET)

stress-test: $(BUILD_DIR) $(BUILD_SUBDIRS) $(TEST_BUILD_DIR) $(STRESS_TEST_TARGET)
	$(STRESS_TEST_TARGET) stress-test.log

start: $(BUILD_DIR) $(BUILD_SUBDIRS) $(TARGET)
	WARUDO_CORS="*" WARUDO_LOG_LEVEL=3 $(TARGET)

$(HC_TREE_BUILD):
	mkdir -p $@

$(HC_TREE_SRC):
	mkdir -p $@

$(HC_TREE_REPO):
	fossil clone https://www.sqlite.org/hctree $@

hctree-pull: $(HC_TREE_BUILD) $(HC_TREE_SRC) $(HC_TREE_REPO)
	cd $(HC_TREE_SRC) && fossil open ../$(HC_TREE_REPO) && fossil up hctree

hctree-patch:
	patch -i hct_file.diff $(HC_TREE_SRC)/src/hct_file.c

hctree-restore:
	patch -R -i hct_file.diff $(HC_TREE_SRC)/src/hct_file.c

hctree-configure:
	cd $(HC_TREE_BUILD) && ../$(HC_TREE_SRC)/configure

hctree-amalgamation: hctree-configure
	rm -rf $(HC_TREE_BUILD)
	mkdir -p $(HC_TREE_BUILD)
	cd $(HC_TREE_BUILD) && ../$(HC_TREE_SRC)/configure
	cd $(HC_TREE_BUILD) && make sqlite3.c
	cp $(HC_TREE_BUILD)/sqlite3.c $(HC_TREE_AMALGAMATION_DIR)/sqlite3.c
	cp $(HC_TREE_BUILD)/sqlite3.h $(HC_TREE_AMALGAMATION_DIR)/sqlite3.h

hctree-make: hctree-configure
	cd $(HC_TREE_BUILD) && make

hctree-clean: $(HC_TREE_BUILD)
	rm -r $(HC_TREE_BUILD)/{*,.*}

db:
	sqlite3 warudo.db

clean:
	rm -rf $(BUILD_DIR) $(TEST_BUILD_DIR) $(TARGET) $(HC_TREE_BUILD) $(HC_TREE_SRC)

.PHONY: all clean
