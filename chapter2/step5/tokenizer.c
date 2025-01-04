#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_at.h"
#include "tokenizer.h"

/* token->kind == TK_RESERVED && token->str[0] == op */
/* Consume if the token matches the op and move to the next token. */
bool consume_op(Token** token, char op) {
    if ((*token)->kind != TK_RESERVED || (*token)->str[0] != op) {
        return false;
    }
    (*token) = (*token)->next;
    return true;
}

/* Ensure the current token is `op` and move to the next token. */
void expect_op(char* user_input, Token** token, char op) {
    if ((*token)->kind != TK_RESERVED || (*token)->str[0] != op) {
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
Token* create_token(TokenKind kind, Token* current, char* str) {
    Token* new_token = calloc(1, sizeof(Token));
    new_token->kind = kind;
    new_token->str = str;
    current->next = new_token;
    return new_token;
}

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
        if (strchr("+-*/()", *p)) {
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
