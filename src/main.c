#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token
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
int pos = 0;

// Node
enum {
    // You can use arbitrary ASCII char as node type
    ND_NUM = 256, // Number
};

typedef struct Node {
    int ty;           // Type of node (operator or number)
    struct Node *lhs; // Only used when type is a operator
    struct Node *rhs; // Only used when type is a operator
    int val;          // Only used when type is a number
} NODE;

static NODE*
new_node_op(int op, NODE *lhs, NODE *rhs)
{
    NODE *node = malloc(sizeof(NODE));
    node->ty = op;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}

static NODE*
new_node_num(int num)
{
    NODE *node = malloc(sizeof(NODE));
    node->ty = ND_NUM;
    node->val = num;

    return node;
}

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
static void errors (char *msg, int i) __attribute__ ((__noreturn__));
static void
errors(char *msg, int i)
{
    fprintf(stderr, msg, tokens[i].input);
    exit(1);
}

// i is index
static void error (int i) __attribute__ ((__noreturn__));
static void
error(int i)
{
    errors("Unexpected token %s\n", i);
}

static NODE* mul();
static NODE* term();

/*
 * expr: mul
 *     | mul "+" expr
 *     | mul "-" expr
 */
static NODE*
expr()
{
    NODE *lhs = mul();

    if (tokens[pos].ty == '+') {
        // Consume '+'
        pos++;
        return(new_node_op('+', lhs, expr()));
    }

    if (tokens[pos].ty == '-') {
        // Consume '-'
        pos++;
        return(new_node_op('-', lhs, expr()));
    }

    return lhs;
}

/*
 * mul: term
 *    | term "*" mul
 *    | term "/" mul
 */
static NODE*
mul()
{
    NODE *lhs = term();

    if (tokens[pos].ty == '*') {
        // Consume '*'
        pos++;
        return(new_node_op('*', lhs, mul()));
    }

    if (tokens[pos].ty == '/') {
        // Consume '/'
        pos++;
        return(new_node_op('/', lhs, mul()));
    }

    return lhs;
}

/*
 * term: number
 *     | "(" expr ")"
 */
static NODE*
term()
{
    if (tokens[pos].ty == TK_NUM) {
        return new_node_num(tokens[pos++].val);
    }

    if (tokens[pos].ty == '(') {
        NODE *node;

        // Consume '('
        pos++;
        node = expr();

        if (tokens[pos].ty != ')') {
            errors("')' is expected but '%s' is given.", pos);
        }

        // Consume ')'
        pos++;
        return node;
    }

    errors("Unexpected token '%s' is given.", pos);
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
    // NODE *node = expr();
    expr();

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
