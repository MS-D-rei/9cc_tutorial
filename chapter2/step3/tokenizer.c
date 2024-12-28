#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

/* Global variable. */
/* Use this variable to make linked list. */
Token* token;

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

/* kind == TK_RESERVED && str[0] == op then return true */
bool consume(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op) {
        return false;
    }
    token = token->next;
    return true;
}

/* Get number from current token and go to next one. */
int expect_number() {
    if (token->kind != TK_NUM) {
        error("not number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() { return token->kind == TK_EOF; }

// Modifiy current token->next to add new_token
Token* create_token(TokenKind kind, Token* current, char* str) {
    Token* new_token = calloc(1, sizeof(Token));
    new_token->kind = kind;
    new_token->str = str;
    current->next = new_token;
    return new_token;
}

/* Create token liked list */
/* head->token1->token2->... and return token1 pointer */
Token* tokenize(char* p) {
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

        error("cannot tokenize");
    }

    // To add EOF token to the last token->next
    create_token(TK_EOF, current, p);

    return head.next;
}

void show_token_content(Token* token) {
    printf("token->str: %s\n", token->str);
    printf("token->kind: %i\n", token->kind);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        error("argument should be 1");
    }

    token = tokenize(argv[1]);

    // show_token_content(token);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  mov rax, %d\n", expect_number());

    while (!at_eof()) {
        // show_token_content(token);
        if (consume('+')) {
            // show_token_content(token);
            printf("  add rax, %d\n", expect_number());
            continue;
        }
        if (consume('-')) {
            // show_token_content(token);
            printf("  sub rax, %d\n", expect_number());
            continue;
        }
    }

    printf("  ret\n");

    return EXIT_SUCCESS;
}
