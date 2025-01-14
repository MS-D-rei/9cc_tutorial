#ifndef CODEGEN_H
#define CODEGEN_H

#include "node.h"

void generate_lvalue(Node* node);

void generate_asm_code(Node* node);

#endif // !CODEGEN_H
