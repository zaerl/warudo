CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS = -L/opt/homebrew/opt/fastcgi/lib -lfcgi
LDLIBS = -lfcgi
FCGI_INCLUDE_PATH = /opt/homebrew/opt/fastcgi/include

SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Get all .c files from src/ directory
SRCS = $(wildcard $(SRCDIR)/*.c)

# Generate corresponding .o file names
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Define the final executable name
TARGET = $(BINDIR)/zaerl

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -L$(FCGI_INCLUDE_PATH) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -I$(FCGI_INCLUDE_PATH) $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)/*

.PHONY: all clean

#TARGET = zaerl

#$(TARGET): zaerl.o
#	$(CC) $(CFLAGS) -o $(TARGET) zaerl.o -L$(FCGI_INCLUDE_PATH) $(LDFLAGS) $(LDLIBS)

#zaerl.o: src/zaerl.c
#	$(CC) $(CFLAGS) -c -I$(FCGI_INCLUDE_PATH) src/zaerl.c

#sqlite3.o: src/sqlite3.c
#	$(CC) $(CFLAGS) -c src/sqlite3.c

#clean:
#	rm -f $(TARGET) *.o

#.PHONY: clean
