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
    Node* node = express(user_input, &token);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // printf("  mov rax, %d\n", expect_number(user_input, &token));
    //
    // while (!at_eof(token)) {
    //     if (consume_op(&token, '+')) {
    //         printf("  add rax, %d\n", expect_number(user_input, &token));
    //         continue;
    //     }
    //     if (consume_op(&token, '-')) {
    //         printf("  sub rax, %d\n", expect_number(user_input, &token));
    //         continue;
    //     }
    // }

    generate_asm_code(node);

    printf("  pop rax\n");
    printf("  ret\n");

    return EXIT_SUCCESS;
}
