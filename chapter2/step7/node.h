#ifndef NODE_H
#define NODE_H

#include "tokenizer.h"

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
} NodeKind;

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    int val; // Only used when NodeKind is ND_NUM.
};

Node* create_node(NodeKind kind, Node* lhs, Node* rhs);

Node* create_node_num(int val);

Node* express(char* user_input, Token** token);

Node* mul(char* user_input, Token** token);

Node* unary(char* user_input, Token** token);

Node* primary(char* user_input, Token** token);

void generate_asm_code(Node* node);

#endif // !NODE_H
