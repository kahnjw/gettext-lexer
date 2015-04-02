#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>

/* Type Definitions */
#define COMMENTTYPE 0x01
#define KEYTYPE 0x02
#define STRINGTYPE 0x03

/* State definitions */
#define NONESTATE 0x01
#define COMMENTSTATE 0x02
#define KEYSTATE 0x03
#define STRINGSTATE 0x04


char * copy_str (char * from)
{
    int str_length;
    char * to;

    str_length = strlen(from);
    to = (char *)malloc(str_length);

    strcpy(to, from);

    return to;
}

char * append_to_buffer(char * buffer, char chr)
{
    int buffer_length;

    buffer_length = 0;
    if (buffer == NULL)
        buffer = (char *)malloc(1);
    else {
        buffer_length = strlen(buffer);
        buffer = (char *)realloc(buffer, buffer_length + 2);
    }

    buffer[buffer_length] = chr;
    buffer[buffer_length + 1] = '\0';

    return buffer;
}

struct Token * append(struct Token * tok_arr, int ntokens, int type, char quote,
    char * val)
{
    int new_size;

    new_size = (ntokens + 1) * sizeof(token);

    if (tok_arr == NULL && ntokens == 0)
        tok_arr = (struct Token *)malloc(sizeof(token));
    else if (tok_arr != NULL && ntokens > 0)
        tok_arr = (struct Token *)realloc(tok_arr, new_size);
    else {
        fprintf(stderr, "Fatal Error: Illegal arguments passed to Token append()\n");
        exit(1);
    }

    tok_arr[ntokens].type = type;
    tok_arr[ntokens].quote = quote;

    if (val != NULL)
        tok_arr[ntokens].value = copy_str(val);
    else
        tok_arr[ntokens].value = NULL;

    return tok_arr;
}

void print_token_array(struct Token * token_array, int size)
{
    int i;

    printf("Tokens in array: %d\n", size);
    for(i = 0; i < size; ++i) {
        printf("--------------------------------\n");
        printf("Token: %d\n", i);
        printf("Type: %d\n", token_array[i].type);
        printf("Quote: %d\n", token_array[i].quote);
        printf("Value: %s\n", token_array[i].value);
        printf("---------------------------------\n\n");
    }
}

void free_token_array(struct Token * token_array, int size)
{
    int i;

    for(i = 0; i < size; ++i)
        free(token_array[i].value);
    free(token_array);
    token_array = NULL;
}

struct Token * parse(struct Token * tokens, char * po_string, int * size)
{
    bool escaped;
    char chr;
    char quote;
    size_t i;
    int ntokens;
    int state;
    char * buffer;

    buffer = NULL;
    tokens = NULL;
    ntokens = 0;
    quote = '\0';
    escaped = false;
    state = NONESTATE;

    for (i = 0; i < strlen(po_string); i++) {
        chr = po_string[i];

        switch(state) {
            case NONESTATE:
                if (chr == '\'' || chr == '"') {
                    state = STRINGSTATE;
                    quote = chr;
                } else if (chr == '#') {
                    state = COMMENTSTATE;
                } else if (!isspace(chr)) {
                    state = KEYSTATE;
                    buffer = append_to_buffer(buffer, chr);
                } else if (isspace(chr) && buffer != NULL) {
                    tokens = append(tokens, ntokens, KEYTYPE, '\0', buffer);
                    free(buffer);
                    buffer = NULL;
                    ++ntokens;
                }

                break;

            case COMMENTSTATE:
                if (chr == '\n') {
                    tokens = append(tokens, ntokens, COMMENTTYPE, '\0', buffer);
                    state = NONESTATE;
                    free(buffer);
                    buffer = NULL;
                    ++ntokens;
                } else if (chr != '\r') {
                    buffer = append_to_buffer(buffer, chr);
                }

                break;

            case STRINGSTATE:
                if(escaped) {
                    if (chr == 't') {
                        buffer = append_to_buffer(buffer, '\t');
                    } else if (chr == 'n') {
                        buffer = append_to_buffer(buffer, '\n');
                    } else if (chr == 'r') {
                        buffer = append_to_buffer(buffer, '\r');
                    }

                    escaped = false;

                } else {
                    if(chr == quote) {
                        tokens = append(tokens, ntokens, STRINGTYPE, quote, buffer);
                        free(buffer);
                        buffer = NULL;
                        state = NONESTATE;
                        ++ntokens;
                    } else if (chr == '\\') {
                        escaped = true;
                        break;
                    } else {
                        buffer = append_to_buffer(buffer, chr);
                    }
                    escaped = false;
                }

                break;

            case KEYSTATE:
                if(isalpha(chr) || chr == '-' || chr == '[' || chr == ']') {
                    buffer = append_to_buffer(buffer, chr);
                } else {
                    state = NONESTATE;
                    --i;
                }

                break;
        }
    }

    *size = ntokens;
    free(buffer);
    return tokens;
}
