#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error_at.h"

typedef enum {
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
    TokenKind kind;
    Token* next;
    int val;
    char* str;
};

char* user_input;

Token* token;

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

/* token->kind == TK_RESERVED && token->str[0] == op */
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        return false;
    }
    token = token->next;
    return true;
}

int expect_number() {
    if (token->kind != TK_NUM) {
        // error("not number");
        error_at(user_input, token->str, "not number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() { return token->kind == TK_EOF; }

Token* create_token(TokenKind kind, Token* current, char* str) {
    Token* new_token = calloc(1, sizeof(Token));
    new_token->kind = kind;
    new_token->str = str;
    current->next = new_token;
    return new_token;
}

Token* tokenize() {
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

        // error("cannot tokenize");
        error_at(user_input, p, "cannot tokenize");
    }

    create_token(TK_EOF, current, p);

    return head.next;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        error("argument should be 1");
    }

    user_input = argv[1];

    token = tokenize();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("  add rax, %d\n", expect_number());
            continue;
        }
        if (consume('-')) {
            printf("  sub rax, %d\n", expect_number());
            continue;
        }
    }

    printf("  ret\n");

    return EXIT_SUCCESS;
}
