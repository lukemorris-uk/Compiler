#ifndef COMPILER_H
#define COMPILER_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Type {
    KEYWORD,
    ID,
    INT_LITERAL,
    SEPARATOR,
    OPERATOR,
    TOK_EOF,
} Type;

typedef enum Key {
    IF,
    ELSE,
    WHILE,
} Key;

typedef enum Sep {
    LPAREN,
    RPAREN,
    LCURLY,
    RCURLY,
    SEMI,
} Sep;

typedef enum Op {
    PLUS,
    MINUS,
    EQUALS,
    LESS_THAN,
    GRTR_THAN,
    EQUALSEQUALS,
} Op;

typedef struct Token {
    Type type;
    int linenum;
    union {
        struct {
            int value;
        } INT_LIT;
        struct {
            char *name;
        } ID;
        struct {
            Key value;
        } KEYWORD;
        struct {
            Sep value;
        } SEPARATOR;
        struct {
            Op value;
        } OPERATOR;
    } is;
    char *content;
} Token;

typedef enum {
    AST_START,
    AST_ASSIGN,
    AST_IF_STMT,
    AST_ELSE_STMT,
    AST_WHILE_STMT,
    AST_EXPR,
    AST_ID,
    AST_INT_LITERAL,
} ASTKind;

typedef struct AST AST;

typedef struct StmtList {
    AST **stmtarray;
    int capacity;
    int len;
} StmtList;

typedef struct {
    StmtList stmts;
} start;
typedef struct {
    AST *condition;
    StmtList stmts;
    AST *else_stmt;
} if_stmt;
typedef struct {
    StmtList stmts;
} else_stmt;
typedef struct {
    AST *condition;
    StmtList stmts;
} while_stmt;
typedef struct {
    AST *target;
    AST *value;
} assign;
typedef struct {
    Op op;
    AST *left;
    AST *right;
} bin;

struct AST {
    ASTKind kind;
    Token token;
    union {
        start start;
        assign assign;
        if_stmt if_stmt;
        else_stmt else_stmt;
        while_stmt while_stmt;
        bin bin;
    } is;
};

Token *lexer(FILE *fp, int *token_count);

typedef struct Tokens {
    Token *tokens;
    int token_count;
    int i;
} TokenStream;

AST *parse_stmt(TokenStream *ts);
Token tokenconsume(TokenStream *ts);
Token tokenpeek(TokenStream *ts);
Token tokenexpect(TokenStream *ts, Type type, int value, char *expected);
void append_stmt(StmtList *s, AST *stmt);
AST *ast_start(void);
AST *node_id(TokenStream *ts);
AST *node_literal(TokenStream *ts);
AST *subtree_bin(Op op, AST *left, AST *right);
AST *subtree_assign(AST *target, AST *value);
AST *parse_atom(TokenStream *ts);
AST *subtree_if_stmt(AST *condition, StmtList stmts, AST *else_stmt);
AST *subtree_else_stmt(StmtList stmts);
AST *subtree_while_stmt(AST *condition, StmtList stmts);
AST *parse_aexpr(TokenStream *ts);
AST *parse_rexpr(TokenStream *ts);
AST *parse_else_stmt(TokenStream *ts);
AST *parse_assign(TokenStream *ts);
AST *parse_if_stmt(TokenStream *ts);
AST *parse_while_stmt(TokenStream *ts);
AST *start_parse(TokenStream *ts);

void ast_print(AST *ast);

typedef enum DataType {
    INT,
} DataType;

typedef struct SymbolTable {
    char **symbols;
    DataType *datatypes;
    int len;
    int capacity;
} SymbolTable;

void ast_analyse(AST *ast, SymbolTable *st);

#endif
