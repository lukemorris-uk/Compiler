#include "compiler.h"

int main(int argc, char **argv) {
    if (argc == 0) {
        exit(EXIT_FAILURE);
    }
    FILE *source = fopen(argv[1], "r");
    int token_count = 0;
    Token *tokens = lexer(source, &token_count);

    TokenStream ts = (TokenStream){tokens, token_count, 0};
    AST *ast = start_parse(&ts);
    ast_print(ast);
    SymbolTable st = (SymbolTable){NULL, NULL, 0, 0};
    ast_analyse(ast, &st);
    FILE *out = fopen("out.s", "w");
    ast_gen(out, ast, &st);

    return EXIT_SUCCESS;
}
