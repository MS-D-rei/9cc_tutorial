#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_at.h"
#include "tokenizer.h"

/* token->kind == TK_RESERVED && token->str[0] == op */
/* Consume if the token matches the op and move to the next token. */
bool consume_op(Token** token, char* op) {
    if ((*token)->kind != TK_RESERVED || (*token)->len != strlen(op) ||
        memcmp((*token)->str, op, (*token)->len)) {
        return false;
    }
    (*token) = (*token)->next;
    return true;
}

/* Ensure the current token is `op` and move to the next token. */
void expect_op(char* user_input, Token** token, char* op) {
    if ((*token)->kind != TK_RESERVED || (*token)->len != strlen(op) ||
        memcmp((*token)->str, op, (*token)->len)) {
        error_at(user_input, (*token)->str, "expected '%c'", op);
    }
    (*token) = (*token)->next;
}

/* Ensure the current token is number and move to the next token then returns the number. */
int expect_number(char* user_input, Token** token) {
    if ((*token)->kind != TK_NUM) {
        error_at(user_input, (*token)->str, "not number");
    }
    int val = (*token)->val;
    *token = (*token)->next;
    return val;
}

bool at_eof(Token* token) { return token->kind == TK_EOF; }

/* Create new token and add it to the `current` token next then returns the new token. */
Token* create_token(TokenKind kind, Token* current, char* str, int len) {
    Token* new_token = calloc(1, sizeof(Token));
    new_token->kind = kind;
    new_token->str = str;
    new_token->len = len;
    current->next = new_token;
    return new_token;
}

/* Return true if input `p` starts with multiletter operation `op` */
bool multiletter_op(char* op, char* p) { return memcmp(op, p, strlen(op)) == 0; }

/* Tokenize `user_input` and returns token linked list. */
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
        if (multiletter_op("==", p) || multiletter_op("!=", p) || multiletter_op("<=", p) ||
            multiletter_op(">=", p)) {
            current = create_token(TK_RESERVED, current, p, 2);
            p += 2;
            continue;
        }
        if (strchr("+-*/()<>", *p)) {
            current = create_token(TK_RESERVED, current, p, 1);
            p++;
            continue;
        }
        if ('a' <= *p && *p <= 'z') {
            current = create_token(TK_IDENT, current, p, 1);
            p++;
            continue;
        }
        if (isdigit(*p)) {
            current = create_token(TK_NUM, current, p, 1);
            current->val = strtol(p, &p, 10);
            continue;
        }

        error_at(user_input, p, "cannot tokenize");
    }

    create_token(TK_EOF, current, p, 0);

    return head.next;
}
