#include <stdio.h>
#include "http_header_parse.h"

int main(int argc, char **argv)
{
    char * str = "GET / HTTP/1.1\r\nHost: 127.0.0.1:12345\r\nUser-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate, br\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nSec-Fetch-Dest: document\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-Site: none\r\nSec-Fetch-User: ?1\r\n\r\n";
    char buf[1024];
    int r = http_header_get(str, (argc > 1)? argv[1] : "Host", buf, 1024);
    printf("%s\n", (r > 0)? buf : "Failed");



    return 0;
}
