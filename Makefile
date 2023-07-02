CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS = -L/opt/homebrew/opt/fastcgi/lib -lfcgi
LDLIBS = -lfcgi
FCGI_INCLUDE_PATH = /opt/homebrew/opt/fastcgi/include

TARGET = j6251

$(TARGET): j6251.o
	$(CC) $(CFLAGS) -o $(TARGET) j6251.o -L$(FCGI_INCLUDE_PATH) $(LDFLAGS) $(LDLIBS)

j6251.o: src/j6251.c
	$(CC) $(CFLAGS) -c -I$(FCGI_INCLUDE_PATH) src/j6251.c

sqlite3.o: src/sqlite3.c
	$(CC) $(CFLAGS) -c src/sqlite3.c

clean:
	rm -f $(TARGET) *.o
