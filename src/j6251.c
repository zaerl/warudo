#include <fcgi_stdio.h>

int main(void) {
    while (FCGI_Accept() >= 0) {
        printf("Content-type: text/html\r\n\r\n");
        printf("<html>\n");
        printf("<head><title>FastCGI Hello World</title></head>\n");
        printf("<body>\n");
        printf("<h1>Hello, FastCGI!!</h1>\n");
        printf("</body>\n");
        printf("</html>\n");
    }

    return 0;
}
