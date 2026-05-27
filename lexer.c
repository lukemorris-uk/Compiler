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
    EXIT,
} Key;

typedef enum Sep {
    LPAREN,
    RPAREN,
    LCURLY,
    RCURLY,
} Sep;

typedef enum Op {
    PLUS,
    MINUS,
    EQUALS,
    LESS_THAN,
    GRTR_THAN,
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

char charpeek(char *str, int i) {
    return str[i];
}

char charconsume(char *str, int *i) {
    return str[(*i)++];
}

Token *lexer(FILE *fp, int *token_count) {
    Token *tokens = NULL;
    char *file, c;
    size_t i;
    FILE *stream = open_memstream(&file, &i);

    while (c = fgetc(fp), c != EOF) {
        fputc(c, stream);
    }
    fputc('\0', stream);
    fclose(stream);

    int file_index = 0;

    while (charpeek(file, file_index) != '\0') {
        if (isalpha(charpeek(file, file_index))) {
            char *token;
            size_t i = 0;
            FILE *strtoken = open_memstream(&token, &i);
            fputc(charconsume(file, &file_index), strtoken);

            while (isalnum(charpeek(file, file_index))) {
                fputc(charconsume(file, &file_index), strtoken);
            }

            fclose(strtoken);

            if (strcmp(token, "if") == 0) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .is.KEYWORD.value = IF};
            } else if (strcmp(token, "else")) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .is.KEYWORD.value = ELSE};
            } else if (strcmp(token, "while")) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .is.KEYWORD.value = WHILE};
            } else if (strcmp(token, "exit")) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .is.KEYWORD.value = EXIT};
            } else {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = ID, .is.ID.name = token};
            }

        } else if (isdigit(charpeek(file, file_index))) {
            char *token;
            size_t i = 0;

            FILE *numtoken = open_memstream(&token, &i);
            fputc(charconsume(file, &file_index), numtoken);

            while (isdigit(charpeek(file, file_index))) {
                fputc(charconsume(file, &file_index), numtoken);
            }

            fclose(numtoken);

            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = INT_LIT, .is.INT_LIT.value = strtoul(token, NULL, 10)};
        } else if (charpeek(file, file_index) == '(') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .is.SEPARATOR.value = LPAREN};
        } else if (charpeek(file, file_index) == ')') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .is.SEPARATOR.value = RPAREN};
        } else if (charpeek(file, file_index) == '{') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .is.SEPARATOR.value = LCURLY};
        } else if (charpeek(file, file_index) == '}') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .is.SEPARATOR.value = RCURLY};
        } else if (charpeek(file, file_index) == '+') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = PLUS};
        } else if (charpeek(file, file_index) == '-') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = MINUS};
        } else if (charpeek(file, file_index) == '=') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = EQUALS};
        } else if (charpeek(file, file_index) == '<') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = LESS_THAN};
        } else if (charpeek(file, file_index) == '>') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = GRTR_THAN};
        } else if (isspace(charpeek(file, file_index))) {
            charconsume(file, &file_index);
        } else {
            fprintf(stderr, "could not tokenize");
            exit(EXIT_FAILURE);
        }
    }

    return tokens;
}
