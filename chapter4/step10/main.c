#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "node.h"
#include "tokenizer.h"

char* user_input;

Token* token;

int main(int argc, char** argv) {
    if (argc != 2) {
        error("argument should be 1");
    }

    user_input = argv[1];
    /* Create token linked list. */
    token = tokenize(user_input);
    /* Create nodes of a abstract syntax tree. */
    Node* code[100];
    program(user_input, &token, code);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    /* Prologue. */
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    /* Generate code from code[0]. */
    for (int i = 0; code[i]; i++) {
        generate_asm_code(code[i]);

        /* Always ends with `push rax`, so apply `pop` not to overflow stack. */
        printf("  pop rax\n");
    }

    /* Epilogue. */
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");

    return EXIT_SUCCESS;
}
