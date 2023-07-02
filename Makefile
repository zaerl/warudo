CC = gcc
CFLAGS = -Wall -Wextra -pedantic
LDFLAGS = -L/opt/homebrew/opt/fastcgi/lib -lfcgi
FCGI_INCLUDE_PATH = /opt/homebrew/opt/fastcgi/include

TARGET = j6251

$(TARGET): j6251.o
	$(CC) $(CFLAGS) -o $(TARGET) j6251.o -L$(FCGI_INCLUDE_PATH) $(LDFLAGS)

j6251.o: src/j6251.c
	$(CC) $(CFLAGS) -c -I$(FCGI_INCLUDE_PATH) src/j6251.c

clean:
	rm -f $(TARGET) *.o
