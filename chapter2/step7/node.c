#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "tokenizer.h"

/* Recursive Decendent Parsing */
/* Express by Backus-Naur form (BNF) */

/* Algorithm for converting Tokens to abstract syntax tree. */

/* Precedence low to high
 *
 * 1. ==, !=
 * 2. <, <=, >, >=
 * 3. +, -
 * 4. *, /
 * 5. +num, -num
 * 6. ()
 *
 */

/*
 * express = equality
 * equality = relational ("==" relational | "!=" relational)*
 * relational = add ("<" add | "<=" add | ">" add | ">=" add)*
 * add = mul ("+" mul | "-" mul)*
 * mul = unary ("*" unary | "/" unary)*
 * unary = ("+" | "-")? primary
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

Node* express(char* user_input, Token** token) { return equality(user_input, token); }

/* equality = relational ("==" relational | "!=" relational)* */
Node* equality(char* user_input, Token** token) {
    Node* node = relational(user_input, token);

    for (;;) {
        if (consume_op(token, "==")) {
            node = create_node(ND_EQ, node, relational(user_input, token));
        } else if (consume_op(token, "!=")) {
            node = create_node(ND_NEQ, node, relational(user_input, token));
        } else {
            return node;
        }
    }
}

/* relational = add ("<" add | "<=" add | ">" add | ">=" add)* */
Node* relational(char* user_input, Token** token) {
    Node* node = add(user_input, token);

    for (;;) {
        if (consume_op(token, "<")) {
            node = create_node(ND_LT, node, add(user_input, token));
        } else if (consume_op(token, "<=")) {
            node = create_node(ND_LTE, node, add(user_input, token));
        } else if (consume_op(token, ">")) {
            node = create_node(ND_LT, add(user_input, token), node);
        } else if (consume_op(token, ">=")) {
            node = create_node(ND_LTE, add(user_input, token), node);
        } else {
            return node;
        }
    }
}

/* add = mul ("+" mul | "-" mul)* */
Node* add(char* user_input, Token** token) {
    Node* node = mul(user_input, token);

    for (;;) {
        if (consume_op(token, "+")) {
            node = create_node(ND_ADD, node, mul(user_input, token));
        } else if (consume_op(token, "-")) {
            node = create_node(ND_SUB, node, mul(user_input, token));
        } else {
            return node;
        }
    }
}

/* mul = unary ("*" unary | "/" unary)* */
Node* mul(char* user_input, Token** token) {
    Node* node = unary(user_input, token);

    for (;;) {
        if (consume_op(token, "*")) {
            node = create_node(ND_MUL, node, unary(user_input, token));
        } else if (consume_op(token, "/")) {
            node = create_node(ND_DIV, node, unary(user_input, token));
        } else {
            return node;
        }
    }
}

/* unary = ("+" | "-")? primary */
Node* unary(char* user_input, Token** token) {
    if (consume_op(token, "+")) {
        return primary(user_input, token);
    }
    /* return a node that has 0-primary() */
    if (consume_op(token, "-")) {
        return create_node(ND_SUB, create_node_num(0), primary(user_input, token));
    }
    return primary(user_input, token);
}

/* primary = num | "(" express ")" */
Node* primary(char* user_input, Token** token) {
    if (consume_op(token, "(")) {
        Node* node = express(user_input, token);
        expect_op(user_input, token, ")");
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
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NEQ:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LTE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}
