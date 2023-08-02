CC=gcc

test: http_header_parse_test
	./http_header_parse_test $(arg)

http_header_parse_test: http_header_parse.c test.c
	$(CC) http_header_parse.c test.c -o http_header_parse_test -Wall
