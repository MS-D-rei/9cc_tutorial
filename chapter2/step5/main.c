#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"

char* user_input;

Token* token;

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        error("argument should be 1");
    }

    user_input = argv[1];

    token = tokenize(user_input);

    // printf("%p\n", (void*) token);
    // printf("%p\n", (void*) &token);
    // printf("%p\n", (void*) token->next);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  mov rax, %d\n", expect_number(user_input, &token));

    // printf("%p\n", (void*) token);

    while (!at_eof(token)) {
        if (consume(&token, '+')) {
            printf("  add rax, %d\n", expect_number(user_input, &token));
            continue;
        }
        if (consume(&token, '-')) {
            printf("  sub rax, %d\n", expect_number(user_input, &token));
            continue;
        }
    }

    printf("  ret\n");

    return EXIT_SUCCESS;
}
