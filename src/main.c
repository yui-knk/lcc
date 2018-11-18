#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum {
    // You can use arbitrary ASCII char as token type
    TK_NUM = 256, // Number
    TK_EOF, // End Of File
};

typedef struct {
    int ty;      // Type of token
    int val;     // Value of token when token is TK_NUM
    char *input; // Token string (for debug)
} token_t;

#define TOKENS_LEN 100

token_t tokens[TOKENS_LEN];


static void
tokenize(char *p) {
    int i = 0;

    while (*p) {
        if (i >= TOKENS_LEN) {
            fprintf(stderr, "Token overflow\n");
            exit(1);
        }

        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            p++;
            i++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "Can not tokenize '%s'\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

// i is index
static void
error(int i)
{
    fprintf(stderr, "Unexpected token %s\n", tokens[i].input);
    exit(1);
}

int
main(int argc, char** argv)
{
    int i;

    if (argc != 2) {
        fprintf(stderr, "One argument should be passed (%d is passed).\n", argc - 1);
        return 1;
    }

    tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n\n");
    printf("main:\n");

    if (tokens[0].ty != TK_NUM) error(0);
    printf("        mov rax, %d\n", tokens[0].val);

    i = 1;

    while (tokens[i].ty != TK_EOF) {
        if (tokens[i].ty == '+') {
            // Consume '+'
            i++;

            if (tokens[i].ty != TK_NUM) error(i);
            printf("        add rax, %d\n", tokens[i].val);
            // Consume NUM
            i++;
            continue;
        }

        if (tokens[i].ty == '-') {
            // Consume '-'
            i++;

            if (tokens[i].ty != TK_NUM) error(i);
            printf("        sub rax, %d\n", tokens[i].val);
            // Consume NUM
            i++;
            continue;
        }

        error(i);
    }


    printf("        ret\n");
    return 0;
}
