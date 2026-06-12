#include "compiler.h"

int main(int argc, char **argv) {
    FILE *source = fopen(argv[1], "r");
    int token_count = 0;
    Token *tokens = lexer(source, &token_count);

    TokenStream ts = (TokenStream){tokens, token_count, 0};
    AST *ast = start_parse(&ts);
    ast_print(ast);

    return EXIT_SUCCESS;
}
