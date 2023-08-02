/* HTTP Header Parse API */
#ifndef HTTP_HEADER_PARSE_H
#define HTTP_HEADER_PARSE_H

#define EVIL_NEWLINES 1

int http_header_get(char *req_str, const char *name, char *buf, size_t buf_len);

#endif
