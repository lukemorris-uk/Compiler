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
    int c;
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

            printf("%s\n", token);

            if (strcmp(token, "if") == 0) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .is.KEYWORD.value = IF};
                free(token);
            } else if (strcmp(token, "else") == 0) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .is.KEYWORD.value = ELSE};
                free(token);
            } else if (strcmp(token, "while") == 0) {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = KEYWORD, .is.KEYWORD.value = WHILE};
                free(token);
            } else {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = ID, .is.ID.name = token};
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
            tokens[*token_count - 1] = (Token){.type = INT_LIT, .is.INT_LIT.value = strtoul(token, NULL, 10)};

            printf("%s\n", token);

            free(token);
        } else if ((unsigned char)charpeek(file, file_index) == '(') {
            printf("%c\n", charconsume(file, &file_index));
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .is.SEPARATOR.value = LPAREN};
        } else if ((unsigned char)charpeek(file, file_index) == ')') {
            printf("%c\n", charconsume(file, &file_index));
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .is.SEPARATOR.value = RPAREN};
        } else if ((unsigned char)charpeek(file, file_index) == '{') {
            printf("%c\n", charconsume(file, &file_index));
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .is.SEPARATOR.value = LCURLY};
        } else if ((unsigned char)charpeek(file, file_index) == '}') {
            printf("%c\n", charconsume(file, &file_index));
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .is.SEPARATOR.value = RCURLY};
        } else if ((unsigned char)charpeek(file, file_index) == ';') {
            printf("%c\n", charconsume(file, &file_index));
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = SEPARATOR, .is.SEPARATOR.value = SEMI};
        } else if ((unsigned char)charpeek(file, file_index) == '+') {
            printf("%c\n", charconsume(file, &file_index));
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = PLUS};
        } else if ((unsigned char)charpeek(file, file_index) == '-') {
            printf("%c\n", charconsume(file, &file_index));
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = MINUS};
        } else if ((unsigned char)charpeek(file, file_index) == '=') {
            charconsume(file, &file_index);
            if ((unsigned char)charpeek(file, file_index) == '=') {
                charconsume(file, &file_index);
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = EQUALITY};
                printf("==\n");
            } else {
                tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
                tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = EQUALS};
                printf("=\n");
            }
        } else if ((unsigned char)charpeek(file, file_index) == '<') {
            printf("%c\n", charconsume(file, &file_index));
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = LESS_THAN};
        } else if ((unsigned char)charpeek(file, file_index) == '>') {
            printf("%c\n", charconsume(file, &file_index));
            tokens = realloc(tokens, (++(*token_count)) * sizeof(Token));
            tokens[*token_count - 1] = (Token){.type = OPERATOR, .is.OPERATOR.value = GRTR_THAN};
        } else if (isspace((unsigned char)charpeek(file, file_index))) {
            charconsume(file, &file_index);
        } else {
            fprintf(stderr, "could not tokenize");
            exit(EXIT_FAILURE);
        }
    }

    return tokens;
}
