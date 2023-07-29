CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -O2 -g
LDFLAGS = -L/opt/homebrew/opt/fastcgi/lib
LDLIBS = -lfcgi
FCGI_INCLUDE_PATH = /opt/homebrew/opt/fastcgi/include

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
OBJDIR = obj
DBS = *.db *.db-shm *.db-wal

# Get all the src/ sub-directories
SUBDIRS := $(shell find $(SRC_DIR) -type d)

# Get all .c files from src/ directory
SRCS = $(shell find $(SRC_DIR) -type f -name "*.c")

# Generate corresponding .o file names
OBJS = $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(SRCS:.c=.o))

# Subdirectories to create in the build directory
BUILD_SUBDIRS := $(foreach dir,$(SUBDIRS),$(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(dir)))

# Define the final executable name
TARGET = $(BIN_DIR)/warudo

all: $(BUILD_DIR) $(BUILD_SUBDIRS) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_SUBDIRS):
	mkdir -p $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -L$(FCGI_INCLUDE_PATH) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -I$(FCGI_INCLUDE_PATH) $< -o $@

stress-test:
	$(CC) $(CFLAGS) -o stress-test stress-test.c -L/opt/homebrew/opt/curl/lib -lcurl

clean:
	rm -rf $(BUILD_DIR) $(TARGET) ${DBS}

.PHONY: all clean

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
