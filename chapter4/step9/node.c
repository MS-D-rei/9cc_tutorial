#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
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
 * program = statement*
 * statement = express ";"
 * express = assign
 * assign = equality ("=" assign)?
 * equality = relational ("==" relational | "!=" relational)*
 * relational = add ("<" add | "<=" add | ">" add | ">=" add)*
 * add = mul ("+" mul | "-" mul)*
 * mul = unary ("*" unary | "/" unary)*
 * unary = ("+" | "-")? primary
 * primary = num | ident | "(" express ")"
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

Node* create_node_ident(Token** token) {
    Node* new_node = calloc(1, sizeof(Node));
    new_node->kind = ND_LVAR;
    new_node->offset = ((*token)->str[0] - 'a' + 1) * 8;
    return new_node;
}

/* program = statement* */
void program(char* user_input, Token** token, Node* code[]) {
    int i = 0;
    while (!at_eof(*token)) {
        code[i] = statement(user_input, token);
        i++;
    }
    code[i] = NULL;
}

/* statement = express ";" */
Node* statement(char* user_input, Token** token) {
    Node* node = express(user_input, token);
    expect_op(user_input, token, ";");
    return node;
}

/* express = assign */
Node* express(char* user_input, Token** token) { return assign(user_input, token); }

/* assign = equality ("=" assign)? */
Node* assign(char* user_input, Token** token) {
    Node* node = equality(user_input, token);

    if (consume_op(token, "=")) {
        node = create_node(ND_ASSIGN, node, assign(user_input, token));
    }

    return node;
}

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

/* primary = num | ident | "(" express ")" */
Node* primary(char* user_input, Token** token) {
    if (consume_op(token, "(")) {
        Node* node = express(user_input, token);
        expect_op(user_input, token, ")");
        return node;
    }
    if ('a' <= (*token)->str[0] && (*token)->str[0] <= 'z') {
        return create_node_ident(token);
    }

    return create_node_num(expect_number(user_input, token));
}

void generate_lvalue(Node* node) {
    if (node->kind != ND_LVAR) {
        error("left value is not variable.");
    }

    /* Push variable located at [Base pointer + offset]. */
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
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
    switch (node->kind) {
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        /* Generate_lvalue pushes variable. */
        generate_lvalue(node);

        /* Takes the variable to rax. */
        printf("  pop rax\n");
        /* Copies the addresss value of rax to rax. */
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        generate_lvalue(node->lhs);
        generate_asm_code(node->rhs);

        /* Takes value of generate_asm_code. */
        printf("  pop rdi\n");
        /* Takes address value of generate_lvalue. */
        printf("  pop rax\n");
        /* Copies the value of generate_asm_code to generate_lvalue. */
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    }

    /* Calculate `lhs` and `rhs`, then push each value to stack. */
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
