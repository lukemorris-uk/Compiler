#ifndef COMPILER_H
#define COMPILER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Type {
    KEYWORD,
    ID,
    INT_LIT,
    SEPARATOR,
    OPERATOR,
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
    EQUALITY,
} Op;

typedef struct Token {
    Type type;
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
} Token;

Token *lexer(FILE *fp, int *token_count);

#endif
