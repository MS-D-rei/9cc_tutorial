#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error_at.h"
#include "tokenizer.h"

/* token->kind == TK_RESERVED && token->str[0] == op */
bool consume(Token** token, char op) {
    if ((*token)->kind != TK_RESERVED || (*token)->str[0] != op) {
        return false;
    }
    (*token) = (*token)->next;
    return true;
}

int expect_number(char* user_input, Token** token) {
    if ((*token)->kind != TK_NUM) {
        error_at(user_input, (*token)->str, "not number");
    }
    int val = (*token)->val;
    *token = (*token)->next;
    return val;
}

bool at_eof(Token* token) { return token->kind == TK_EOF; }

Token* create_token(TokenKind kind, Token* current, char* str) {
    Token* new_token = calloc(1, sizeof(Token));
    new_token->kind = kind;
    new_token->str = str;
    current->next = new_token;
    return new_token;
}

Token* tokenize(char* user_input) {
    char* p = user_input;
    Token head;
    head.next = NULL;
    Token* current = &head;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }
        if (*p == '+' || *p == '-') {
            current = create_token(TK_RESERVED, current, p);
            p++;
            continue;
        }
        if (isdigit(*p)) {
            current = create_token(TK_NUM, current, p);
            current->val = strtol(p, &p, 10);
            continue;
        }

        error_at(user_input, p, "cannot tokenize");
    }

    create_token(TK_EOF, current, p);

    return head.next;
}
