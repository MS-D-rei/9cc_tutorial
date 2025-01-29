#include <stdbool.h>
#include <stdio.h>

#include "error.h"
#include "node.h"

void generate_lvalue(Node* node) {
    if (node->kind != ND_LVAR) {
        error("left value is not ND_LVAR.");
    }

    /* Push variable address value located at [Base pointer + offset]. */
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->lvar->offset);
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
        /* Generate_lvalue pushes variable address value to the bottom of the stack. */
        generate_lvalue(node);

        /* Takes the address value to rax. */
        printf("  pop rax\n");
        /* Copies the value which the address holds of rax to rax. */
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
    case ND_RETURN:
        generate_asm_code(node->lhs);

        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    default:
        break;
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
    default:
        break;
    }

    printf("  push rax\n");
}
