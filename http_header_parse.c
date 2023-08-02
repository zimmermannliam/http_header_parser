/* HTTP Header Parse source code */

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
#include "http_header_parse.h"

int WSP(char *s)
{
    return (s[0] == ' ') || (s[0] == '\t') ? 1 : 0;
}

int CRLF(char *s)
{
    if (s[0] == '\r' && s[1] == '\n')
        return 2;
#if EVIL_NEWLINES
    else if (s[0] == '\n')
        return 1;
#endif
    return 0;
}

int CRLF_WSP(char *s)
{
    if ((CRLF(s) > 0) && WSP(s + CRLF(s)))
        return CRLF(s) + WSP(s + CRLF(s));
    else
        return 0;
        
}


void gobble_LWS(char **s)
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

int is_new_line(char *substr, char *original_str)
/* True if the substring is the beginning of the original or if it has a newline before it */
{
    return ((substr == original_str) 
    || ((substr > original_str) && (*(substr - 1) == '\n')));
}

size_t strlncpy(char *dest, char *src, size_t max)
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

int http_header_get(char *hdr_str, const char *name, char *buf, size_t buf_len)
{
    char *key_line;
    char *field;

    // Grab the line with the key
    while ((key_line = strstr(hdr_str, name)))
    {
        if (is_new_line(key_line, hdr_str))
            break;
        else
            key_line += 1; // Don't re-match self
    }

    // No key found
    if (!key_line)
        return -1;
    
    // Go to after ': <LWS>'
    field = strchr(key_line, ':');

    // This shouldn't happen with good formatting, invalid
    if (!field)
        return -2;
    
    // Get rid of colon and spaces
    ++field;
    gobble_LWS(&field);

    return strlncpy(buf, field, buf_len);
}
