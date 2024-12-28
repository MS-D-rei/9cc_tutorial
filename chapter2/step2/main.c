#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "%s\n", "The number of args should be 1");
        return 1;
    }

    char* p = argv[1];
    /* These lines are for checking pointer movement. */
    // printf("%p\n", (void *)p); // c139
    // printf("%p\n", (void *)&p); // a350
    // printf("%c\n", *p); // 5
    // printf("%p\n", (void*)&(*p)); // c139
    // printf("%p\n", (void *)(p+1)); // c13a

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("  mov rax, %ld\n", strtol(p, &p, 10));

    /* `strtol` moves `p` */
    // printf("%p\n", (void *)p); // c13a
    // printf("%p\n", (void *)&p); // a350

    // Until reach '\0'
    while (*p) {
        if (*p == '+') {
            p++; // go to next pointer
            printf("  add rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        if (*p == '-') {
            p++;
            printf("  sub rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        fprintf(stderr, "Unexpected character");
        return EXIT_FAILURE;
    }

    printf("  ret\n");
    return EXIT_SUCCESS;
}
