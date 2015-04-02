#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
        buffer = (char *)realloc(buffer, buffer_length + 1);
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
        tok_arr[ntokens].value = val;

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
