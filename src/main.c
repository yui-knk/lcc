#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token
enum {
    // You can use arbitrary ASCII char as token type
    TK_NUM = 256, // Number
    TK_IDENT, // Identifier
    TK_EOF, // End Of File
};

typedef struct {
    int ty;      // Type of token
    int val;     // Value of token when token is TK_NUM or TK_IDENT
    char *input; // Token string (for debug)
} token_t;

#define TOKENS_LEN 100

token_t tokens[TOKENS_LEN];
// Index of tokens
int pos = 0;

// Node
enum {
    // You can use arbitrary ASCII char as node type, for example operator.
    ND_NUM = 256, // Number
    ND_IDENT, // Identifier
};

typedef struct Node {
    int ty;           // Type of node
    struct Node *lhs; // Left-hand side
    struct Node *rhs; // Right-hand side
    int val;          // Only used when type is a number
    char name;        // Only used when type is an identifier
} NODE;

#define CODE_LEN 100

// program has many codes.
NODE *code[CODE_LEN];
int code_pos = 0;

static void
push_code(NODE *node)
{
    if (code_pos >= CODE_LEN) {
        fprintf(stderr, "Code overflow\n");
        exit(1);
    }

    code[code_pos] = node;
    code_pos++;
}

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

static NODE*
new_node_lvar(char var)
{
    // lhs and rhs are null when node is variable access.
    NODE *node = malloc(sizeof(NODE));
    node->ty = ND_IDENT;
    node->name = var;

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

        if (*p == '+' || *p == '-' ||
            *p == '*' || *p == '/' ||
            *p == '(' || *p == ')' ||
            *p == '=' || *p == ';') {
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

        if ('a' <= *p && *p <= 'z') {
            tokens[i].ty = TK_IDENT;
            tokens[i].input = p;
            tokens[i].val = *p;
            p++;
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

static void error_node (char *str, NODE *node) __attribute__ ((__noreturn__));
static void
error_node(char *str, NODE *node)
{
    fprintf(stderr, "%s (%c)\n", str, node->ty);
    exit(1);
}

static NODE* mul();
static NODE* term();

static void assign();
static NODE* expr();
static NODE* mul();
static NODE* term();


/*
 * program: assign program'
 * program': ε | assign program'
 */
static void
program()
{
    assign();

    while (tokens[pos].ty != TK_EOF) {
        assign();
    }
}

/*
 * assign: expr assign' ";"
 * assign': ε | "=" expr assign'
 */
static void
assign()
{
    int in_loop = 0;
    NODE *lhs = expr();

    while (tokens[pos].ty == '=') {
        NODE *rhs;

        // Consume '='
        pos++;

        rhs = expr();
        push_code(new_node_op('=', lhs, rhs));
        in_loop = 1;
    }

    if (!in_loop) {
        push_code(lhs);
    }

    if (tokens[pos].ty == ';') {
        // Consume ';'
        pos++;
        return;
    }

    errors("';' is expected, but '%s' is given.\n", pos);
}

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
 *     | ident
 *     | "(" expr ")"
 */
static NODE*
term()
{
    if (tokens[pos].ty == TK_NUM) {
        return new_node_num(tokens[pos++].val);
    }

    if (tokens[pos].ty == TK_IDENT) {
        return new_node_lvar(tokens[pos++].val);
    }

    if (tokens[pos].ty == '(') {
        NODE *node;

        // Consume '('
        pos++;
        node = expr();

        if (tokens[pos].ty != ')') {
            errors("')' is expected but '%s' is given.\n", pos);
        }

        // Consume ')'
        pos++;
        return node;
    }

    errors("Unexpected token '%s' is given.\n", pos);
}

static void
gen_lval(NODE *node)
{
    // Calculate an address of a variable using rax and push the result.
    if (node->ty == ND_IDENT) {
        printf("        mov rax, rbp\n");
        // Stack growth doesn't usually depend on the operating system itself, but on the processor it's running on.
        // x86: down.
        printf("        sub rax, %d\n", ('z' - node->name + 1) * 8);
        printf("        push rax\n");
        return;
    }

    error_node("Left-hand side is not variable.", node);
}

// The end of each codes of a node is "push".
static void
gen(NODE *node)
{
    if (node->ty == ND_NUM) {
        printf("        push %d\n", node->val);
        return;
    }

    // Local variable read
    if (node->ty == ND_IDENT) {
        gen_lval(node);
        printf("        pop rax\n");
        printf("        mov rax, [rax]\n");
        printf("        push rax\n");
        return;
    }

    // Local variable assignment
    if (node->ty == '=') {
        gen_lval(node->lhs);
        gen(node->rhs);

        // 
        printf("        pop rdi\n");
        printf("        pop rax\n");
        printf("        mov [rax], rdi\n");
        printf("        push rdi\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("        pop rdi\n");
    printf("        pop rax\n");

    switch (node->ty) {
      case '+':
        printf("        add rax, rdi\n");
        break;
      case '-':
        printf("        sub rax, rdi\n");
        break;
      case '*':
        printf("        mul rdi\n");
        break;
      case '/':
        printf("        mov rdx, 0\n");
        printf("        div rdi\n");
        break;
    }

    printf("        push rax\n");
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
    program();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n\n");
    printf("main:\n");

    // Prologue
    printf("        push rbp\n");
    printf("        mov rbp, rsp\n");
    printf("        sub rsp, 208\n"); // 26 * 8

    for (i = 0; code[i]; i++) {
        gen(code[i]);

        printf("        pop rax\n");
    }

    // Epilogue
    printf("        mov rsp, rbp\n");
    printf("        pop rbp\n");

    // RAX is return value
    printf("        ret\n");
    return 0;
}
