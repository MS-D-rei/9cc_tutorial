#ifndef NODE_H
#define NODE_H

#include "tokenizer.h"

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,  // `==`
    ND_NEQ, // `!=`
    ND_LT,  // `<`
    ND_LTE, // `<=`
    ND_ASSIGN,
    ND_LVAR,   // Local variable.
    ND_RETURN, // `return`.
    ND_NUM,
} NodeKind;

typedef struct LVar LVar;
struct LVar {
    LVar* next;
    char* name;
    int len;    // Name length.
    int offset; // Stack location.
};

typedef struct Node Node;
struct Node {
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    int val;    // Only used when NodeKind is ND_NUM.
    LVar* lvar; // Only used when NodeKind is ND_LVAR.
};

Node* create_node(NodeKind kind, Node* lhs, Node* rhs);

Node* create_node_num(int val);

Node* create_node_lvar();

Token* create_merged_token_ident(Token** token);

int count_token_letter(Token* token);

char* create_lvar_name(Token* token, int letter_count);

void program(char* user_input, Token** token, Node* code[]);

Node* statement(char* user_input, Token** token);

Node* express(char* user_input, Token** token);

Node* assign(char* user_input, Token** token);

Node* equality(char* user_input, Token** token);

Node* relational(char* user_input, Token** token);

Node* add(char* user_input, Token** token);

Node* mul(char* user_input, Token** token);

Node* unary(char* user_input, Token** token);

Node* primary(char* user_input, Token** token);

LVar* find_lvar(Token* token, LVar* locals);

void assign_lvar_offsets(LVar* locals);

#endif // !NODE_H
