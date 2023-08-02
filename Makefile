CC=gcc

test: http_msg_parse_test
	./http_msg_parse_test $(arg)

http_msg_parse_test: http_msg_parse.c test.c
	$(CC) http_msg_parse.c test.c -o http_msg_parse_test -Wall
