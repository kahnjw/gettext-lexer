#ifndef LEXER_H_
#define LEXER_H_

typedef struct Token {
    int type;
    char * value;
    char quote;
} token;


struct Token * append(struct Token * toks, int ntoks, int t, char q, char * v);
char * append_to_buffer(char * buffer, char chr);
char * copy_str(char * from);
void free_token_array(struct Token * token_array, int size);
void print_token_array(struct Token * token_array, int size);

#endif
