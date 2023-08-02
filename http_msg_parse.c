/* HTTP Message Parse source code */

/* Copyright (c) 2023 Liam Zimmermann
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string.h>
#include "http_msg_parse.h"

static int WSP(char *s)
{
    return (s[0] == ' ') || (s[0] == '\t') ? 1 : 0;
}

static int CRLF(char *s)
{
    if (s[0] == '\r' && s[1] == '\n')
        return 2;
#if EVIL_NEWLINES
    else if (s[0] == '\n')
        return 1;
#endif
    return 0;
}

static int CRLF_WSP(char *s)
{
    if ((CRLF(s) > 0) && WSP(s + CRLF(s)))
        return CRLF(s) + WSP(s + CRLF(s));
    else
        return 0;
        
}


static void gobble_LWS(char **s)
{
    for (;;)
    {
        if (WSP(*s))
            *s += WSP(*s);
        else if (CRLF_WSP(*s))
            *s += CRLF_WSP(*s);
        else break;
    }
}

static int is_new_line(char *substr, char *original_str)
/* True if the substring is the beginning of the original or if it has a newline before it */
{
    return ((substr == original_str) 
    || ((substr > original_str) && (*(substr - 1) == '\n')));
}

static size_t strlncpy(char *dest, char *src, size_t max)
/* Copy a line up to 'max', return amount copied */
{
    size_t i;
    for (i = 0; (i < max) && (src[i] != '\0') && (src[i] != '\n') && (src[i] != '\r'); ++i)
    {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return i;
}

static size_t strlnlen(char *str)
/* Return the length of the string or the line */
{
    int i;
    for (i = 0; (str[i] != '\0') && (str[i] != '\n') && (str[i] != '\r'); ++i); // Do nothing
    return i;
}

int http_msg_get_hdr_field_val(char *msg_str, const char *field_name, char *buf, size_t buf_len)
/* Get a field_value in hdr_str by field_name. And put it in buf, up to a
 * maximum of buf_len. Return -1 if not found, -2 if invalid formatting, or the
 * length of the field otherwise. Note that if the field is longer than the
 * buf_len that can be checked by comparing the return value to buf_len. */
{
    char *key_line;
    char *field_value;

    // Grab the line with the key
    while ((key_line = strstr(msg_str, field_name)))
    {
        if (is_new_line(key_line, msg_str))
            break;
        else
            key_line += 1; // Don't re-match self
    }

    // No key found
    if (!key_line)
        return -1;
    
    // Go to after ': <LWS>'
    field_value = strchr(key_line, ':');

    // This shouldn't happen with good formatting, invalid
    if (!field_value)
        return -2;
    
    // Get rid of colon and spaces
    ++field_value;
    gobble_LWS(&field_value);

    strlncpy(buf, field_value, buf_len);
    return strlnlen(field_value);
}

int http_msg_get_method(char *msg_str)
{
    char *msg_ptr;
    
    // Gobble newlines
    for (msg_ptr = msg_str; (*msg_ptr == '\r') || (*msg_ptr == '\n'); ++msg_ptr); // Do nothing

    if (strncmp(msg_ptr, "OPTIONS", strlen("OPTIONS")))
        return HTTP_METHOD_OPTIONS;
    else if (strncmp(msg_ptr, "GET", strlen("GET")))
        return HTTP_METHOD_GET;
    else if (strncmp(msg_ptr, "POST", strlen("POST")))
        return HTTP_METHOD_POST;
    else if (strncmp(msg_ptr, "PUT", strlen("PUT")))
        return HTTP_METHOD_PUT;
    else if (strncmp(msg_ptr, "DELETE", strlen("DELETE")))
        return HTTP_METHOD_DELETE;
    else if (strncmp(msg_ptr, "TRACE", strlen("TRACE")))
        return HTTP_METHOD_TRACE;
    else if (strncmp(msg_ptr, "CONNECT", strlen("CONNECT")))
        return HTTP_METHOD_CONNECT;
    return -1;
}
