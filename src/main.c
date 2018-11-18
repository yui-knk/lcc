#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char** argv)
{
    char *p = argv[1];

    if (argc != 2) {
        fprintf(stderr, "One argument should be passed (%d is passed).\n", argc - 1);
        return 1;
    }

    printf(".intel_syntax noprefix\n");
    printf(".global main\n\n");
    printf("main:\n");
    printf("        mov rax, %ld\n", strtol(p, &p, 10));

    while (*p) {
        if (*p == '+') {
            p++;
            printf("        add rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        if (*p == '-') {
            p++;
            printf("        sub rax, %ld\n", strtol(p, &p, 10));
            continue;
        }

        fprintf(stderr, "Unexpected char '%c'\n", *p);
        return 1;
    }

    printf("        ret\n");
    return 0;
}
