#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void error(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char* user_input, char* location, char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int position = location - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", position, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}
