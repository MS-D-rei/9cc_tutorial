#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token* next;
    int val;
    char* str;
    int len;
};

bool consume_op(Token** token, char* op);

void expect_op(char* user_input, Token** token, char* op);

int expect_number(char* user_input, Token** token);

bool at_eof(Token* token);

Token* create_token(TokenKind kind, Token* current, char* str, int len);

bool multiletter_op(char* op, char* p);

Token* tokenize(char* user_input);

#endif // !TOKENIZER_H
