#include <stdlib.h>
#include <stdbool.h>

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

bool consume_node(char op) {
}

void expect_node_op(char op) {}

int expect_node_num() {}

Node* express() {
    Node* node = mul();

    for(;;) {
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

    for(;;) {
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
