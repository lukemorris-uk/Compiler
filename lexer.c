#include "compiler.h"

char charpeek(char *str, int i) {
    return str[i];
}

char charconsume(char *str, int *i) {
    return str[(*i)++];
}

Token *lexer(FILE *fp, int *token_count) {
    Token *tokens = NULL;
    char *file;
    int c, linenum = 1;
    size_t i;
    FILE *stream = open_memstream(&file, &i);

    while (c = fgetc(fp), c != EOF) {
        fputc(c, stream);
    }
    fputc('\0', stream);
    fclose(stream);

    int file_index = 0;

    while (charpeek(file, file_index) != '\0') {
        if (isalpha((unsigned char)charpeek(file, file_index))) {
            char *token;
            size_t i = 0;
            FILE *strtoken = open_memstream(&token, &i);
            fputc(charconsume(file, &file_index), strtoken);

            while (isalnum((unsigned char)charpeek(file, file_index))) {
                fputc(charconsume(file, &file_index), strtoken);
            }

            fclose(strtoken);

            if (strcmp(token, "if") == 0) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .linenum = linenum, .is.KEYWORD.value = IF, .content = "if"};
                free(token);
            } else if (strcmp(token, "else") == 0) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .linenum = linenum, .is.KEYWORD.value = ELSE, .content = "else"};
                free(token);
            } else if (strcmp(token, "while") == 0) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .linenum = linenum, .is.KEYWORD.value = WHILE, .content = "while"};
                free(token);
            } else if (strcmp(token, "output") == 0) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .linenum = linenum, .is.KEYWORD.value = OUTPUT, .content = "output"};
                free(token);
            } else {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = ID, .linenum = linenum, .is.ID.name = token, .content = token};
            }

        } else if (isdigit((unsigned char)charpeek(file, file_index))) {
            char *token;
            size_t i = 0;

            FILE *numtoken = open_memstream(&token, &i);
            fputc(charconsume(file, &file_index), numtoken);

            while (isdigit((unsigned char)charpeek(file, file_index))) {
                fputc(charconsume(file, &file_index), numtoken);
            }

            fclose(numtoken);

            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = INT_LITERAL, .linenum = linenum, .is.INT_LIT.value = strtoul(token, NULL, 10), .content = token};

        } else if ((unsigned char)charpeek(file, file_index) == '(') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .linenum = linenum, .is.SEPARATOR.value = LPAREN, .content = "("};
        } else if ((unsigned char)charpeek(file, file_index) == ')') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .linenum = linenum, .is.SEPARATOR.value = RPAREN, .content = ")"};
        } else if ((unsigned char)charpeek(file, file_index) == '{') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .linenum = linenum, .is.SEPARATOR.value = LCURLY, .content = "{"};
        } else if ((unsigned char)charpeek(file, file_index) == '}') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .linenum = linenum, .is.SEPARATOR.value = RCURLY, .content = "}"};
        } else if ((unsigned char)charpeek(file, file_index) == ';') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .linenum = linenum, .is.SEPARATOR.value = SEMI, .content = ";"};
        } else if ((unsigned char)charpeek(file, file_index) == '+') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .linenum = linenum, .is.OPERATOR.value = PLUS, .content = "+"};
        } else if ((unsigned char)charpeek(file, file_index) == '-') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .linenum = linenum, .is.OPERATOR.value = MINUS, .content = "-"};
        } else if ((unsigned char)charpeek(file, file_index) == '=') {
            charconsume(file, &file_index);
            if ((unsigned char)charpeek(file, file_index) == '=') {
                charconsume(file, &file_index);
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = OPERATOR, .linenum = linenum, .is.OPERATOR.value = EQUALSEQUALS, .content = "=="};
            } else {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = OPERATOR, .linenum = linenum, .is.OPERATOR.value = EQUALS, .content = "="};
            }
        } else if ((unsigned char)charpeek(file, file_index) == '<') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .linenum = linenum, .is.OPERATOR.value = LESS_THAN, .content = "<"};
        } else if ((unsigned char)charpeek(file, file_index) == '>') {
            charconsume(file, &file_index);
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .linenum = linenum, .is.OPERATOR.value = GRTR_THAN, .content = ">"};
        } else if (isspace((unsigned char)charpeek(file, file_index))) {
            if (charconsume(file, &file_index) == '\n') {
                linenum++;
            }
        } else {
            fprintf(stderr, "line %d: invalid syntax\n", linenum);
            exit(EXIT_FAILURE);
        }
    }

    tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
    tokens[*token_count - 1] = (Token){.type = TOK_EOF, .linenum = linenum, .content = "end of file"};

    return tokens;
}
