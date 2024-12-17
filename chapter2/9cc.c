/* Usage */

/* gcc-11 -o 9cc 9cc.c */
/* ./9cc [number] > tmp.s */

/* gcc-11 -o tmp tmp.s */
/* ./tmp */
/* echo $? => return [number] */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        // fprintf(FILE *, const char *, ...)
        fprintf(stderr, "Number of arguments should be 2\n");
        return 1;
    }

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf(" mov rax, %d\n", atoi(argv[1]));
    printf(" ret\n");
    return 0;
}
