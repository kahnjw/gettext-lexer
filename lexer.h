#ifndef LEXER_H_
#define LEXER_H_

typedef struct Token {
    int type;
    char * value;
    char quote;
} token;


struct Token * append(struct Token * tok_arr, int ntokens, int type, char quote,
    char * val);
char * append_to_buffer(char * buffer, char chr);
char * copy_str(char * from);
void free_token_array(struct Token * token_array, int size);
void print_token_array(struct Token * token_array, int size);

#endif
