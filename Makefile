CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS = -L/opt/homebrew/opt/fastcgi/lib -lfcgi
LDLIBS = -lfcgi
FCGI_INCLUDE_PATH = /opt/homebrew/opt/fastcgi/include

SRCDIR = src
OBJDIR = obj
BINDIR = bin
DBS = *.db *.db-shm *.db-wal

# Get all .c files from src/ directory
SRCS = $(wildcard $(SRCDIR)/*.c)

# Generate corresponding .o file names
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Define the final executable name
TARGET = $(BINDIR)/warudo

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -L$(FCGI_INCLUDE_PATH) $(LDFLAGS) $(LDLIBS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -I$(FCGI_INCLUDE_PATH) $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

stress-test:
	$(CC) $(CFLAGS) -o stress-test stress-test.c -L/opt/homebrew/opt/curl/lib -lcurl

clean:
	rm -rf $(OBJDIR) $(BINDIR)/* ${DBS}

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
