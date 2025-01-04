#ifndef NODE_H
#define NODE_H

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

#endif // !NODE_H
