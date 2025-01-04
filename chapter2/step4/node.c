#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "node.h"

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

bool consume_node(char op) {}

void expect_node_op(char op) {}

int expect_node_num() {}

Node* express() {
    Node* node = mul();

    for (;;) {
        if (consume_node('+')) {
            node = create_node(ND_ADD, node, mul());
        } else if (consume_node('-')) {
            node = create_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

Node* mul() {
    Node* node = primary();

    for (;;) {
        if (consume_node('*')) {
            node = create_node(ND_MUL, node, primary());
        } else if (consume_node('/')) {
            node = create_node(ND_DIV, node, primary());
        } else {
            return node;
        }
    }
}

Node* primary() {
    if (consume_node('(')) {
        Node* node = express();
        expect_node_op(')');
        return node;
    }

    return create_node_num(expect_node_num());
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
void generate(Node* node) {
    if (node->kind == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    /* calculate `lhs` and `rhs`, then push each value to stack. */
    generate(node->lhs);
    generate(node->rhs);

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
