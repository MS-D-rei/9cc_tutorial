#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "tokenizer.h"

/* Recursive Decendent Parsing */
/* Express by Backus-Naur form (BNF) */

/* Algorithm for converting Tokens to abstract syntax tree. */

/*
 * express = mul ("+" mul | "-" mul)*
 * mul = primary ("*" primary | "/" primary)*
 * primary = num | "(" express ")"
 */

Node* create_node(NodeKind kind, Node* lhs, Node* rhs) {
    Node* new_node = calloc(1, sizeof(Node));
    new_node->kind = kind;
    new_node->lhs = lhs;
    new_node->rhs = rhs;
    return new_node;
}

Node* create_node_num(int val) {
    Node* new_node = calloc(1, sizeof(Node));
    new_node->kind = ND_NUM;
    new_node->val = val;
    return new_node;
}

Node* express(char* user_input, Token** token) {
    Node* node = mul(user_input, token);

    for (;;) {
        if (consume_op(token, '+')) {
            node = create_node(ND_ADD, node, mul(user_input, token));
        } else if (consume_op(token, '-')) {
            node = create_node(ND_SUB, node, mul(user_input, token));
        } else {
            return node;
        }
    }
}

Node* mul(char* user_input, Token** token) {
    Node* node = primary(user_input, token);

    for (;;) {
        if (consume_op(token, '*')) {
            node = create_node(ND_MUL, node, primary(user_input, token));
        } else if (consume_op(token, '/')) {
            node = create_node(ND_DIV, node, primary(user_input, token));
        } else {
            return node;
        }
    }
}

Node* primary(char* user_input, Token** token) {
    if (consume_op(token, '(')) {
        Node* node = express(user_input, token);
        expect_op(user_input, token, ')');
        return node;
    }

    return create_node_num(expect_number(user_input, token));
}

/*
 * ex. calculation for `2*3+4*5`
 *
 * calculate `2 * 3` and push to stack.
 * push 2
 * push 3
 * pop rdi
 * pop rax
 * mul rax, rdi
 * push rax
 * calculate `4 * 5` and push to stack.
 * push 4
 * push 5
 * pop rdi
 * pop rax
 * mul rax, rdi
 * push rax
 * Finally, add 2 intermediate results and push to stack.
 * pop rdi
 * pop rax
 * add rax, rdi
 * push rax
 *
 */
/*
 *    A
 *   / \
 * lhs rhs
 */
void generate_asm_code(Node* node) {
    if (node->kind == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    /* calculate `lhs` and `rhs`, then push each value to stack. */
    generate_asm_code(node->lhs);
    generate_asm_code(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        /* https://www.felixcloutier.com/x86/cwd:cdq:cqo */
        /* `CQO` instruction (available in 64-bit mode only) copies the sign (bit63)
         * of the value in the RAX register into every bit position in the RDX register.  */
        printf("  cqo\n");
        /* https://www.tutorialspoint.com/assembly_programming/assembly_arithmetic_instructions.htm
         */
        /* `idiv` does EDX:EAX / 32bit divisor = EAX(Quotient) and EDX(Remainder) */
        printf("  idiv rdi\n");
        break;
    }

    printf("  push rax\n");
}
