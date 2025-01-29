#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * statement = express ";" | "return" express ";"
 * express = assign
 * assign = equality ("=" assign)?
 * equality = relational ("==" relational | "!=" relational)*
 * relational = add ("<" add | "<=" add | ">" add | ">=" add)*
 * add = mul ("+" mul | "-" mul)*
 * mul = unary ("*" unary | "/" unary)*
 * unary = ("+" | "-")? primary
 * primary = num | ident | "(" express ")"
 */

LVar* locals = NULL;

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

Node* create_node_lvar() {
    Node* new_node = calloc(1, sizeof(Node));
    new_node->kind = ND_LVAR;
    return new_node;
}

/* Merges consecutive ident tokens into one token. */
Token* create_merged_token_ident(Token** token) {
    Token* new_token = calloc(1, sizeof(Token));
    new_token->kind = TK_IDENT;

    Token* current = *token;
    /* Moves token to then token after merged one. */
    while (current->kind == TK_IDENT) {
        current = current->next;
    }
    (*token) = current;

    return new_token;
}

int count_token_letter(Token* token) {
    int count = 0;
    while (token->kind == TK_IDENT) {
        count++;
        token = token->next;
    }

    return count;
}

char* create_lvar_name(Token* token, int letter_count) {
    char* str = calloc(letter_count + 1, sizeof(char));
    for (int i = 0; i < letter_count; i++) {
        str[i] = token->str[i];
    }
    str[letter_count] = '\0';

    return str;
}

/* program = statement* */
void program(char* user_input, Token** token, Node* code[]) {
    int i = 0;
    while (!at_eof(*token)) {
        code[i] = statement(user_input, token);
        i++;
    }
    code[i] = NULL;
    /* After creating all nodes, assign all lvar offsets. */
    assign_lvar_offsets(locals);
}

/* statement = express ";" | "return" express ";" */
Node* statement(char* user_input, Token** token) {
    Node* node;
    if ((*token)->kind == TK_RETURN) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        (*token) = (*token)->next;
        node->lhs = express(user_input, token);

    } else {
        node = express(user_input, token);
    }
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
        /* Create new token to merge TK_IDENT tokens. */
        /* And move token list to the token after the merged token. */
        int letter_count = count_token_letter(*token);
        char* str = create_lvar_name(*token, letter_count);
        Token* merged_token = create_merged_token_ident(token);
        merged_token->str = str;
        merged_token->len = strlen(str);
        Node* node = create_node_lvar();

        LVar* lvar = find_lvar(merged_token, locals);

        if (lvar) {
            node->lvar = lvar;
        } else {
            /* Create new lvar and link to locals. */
            LVar* lvar = calloc(1, sizeof(LVar));
            lvar->name = merged_token->str;
            lvar->len = merged_token->len;
            lvar->next = locals;
            locals = lvar;
            /* Link the node with the lvar. */
            node->lvar = lvar;
        }

        return node;
    }

    return create_node_num(expect_number(user_input, token));
}

LVar* find_lvar(Token* token, LVar* locals) {
    for (LVar* var = locals; var; var = var->next) {
        /* Length and name are the same */
        if (var->len == token->len && !memcmp(token->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}

void assign_lvar_offsets(LVar* locals) {
    int offset = 0;
    for (LVar* var = locals; var; var = var->next) {
        offset += 8;
        var->offset = offset;
    }
}
