#include "compiler.h"

bool in_symboltable(char *id, SymbolTable *st) {
    for (int i = 0; i < st->len; i++) {
        if (strcmp(id, st->symbols[i]) == 0) {
            return true;
        }
    }
    return false;
}

void append_symbol(Token token, SymbolTable *st) {
    if (st->len == st->capacity) {
        st->capacity = st->capacity == 0 ? 8 : st->capacity * 2;
        st->symbols = realloc(st->symbols, st->capacity * sizeof(char *));
        st->datatypes = realloc(st->datatypes, st->capacity * sizeof(DataType));
    }
    st->len += 1;
    st->symbols[st->len - 1] = token.content;
    st->datatypes[st->len - 1] = INT;
}

void ast_analyse_stmtlist(StmtList *list, SymbolTable *st) {
    for (int i = 0; i < list->len; i++) {
        ast_analyse(list->stmtarray[i], st);
    }
}

void ast_analyse_expr(AST *ast, SymbolTable *st) {
    switch (ast->kind) {
    case AST_EXPR:
        ast_analyse_expr(ast->is.bin.left, st);
        ast_analyse_expr(ast->is.bin.right, st);
        break;
    case AST_ID:
        if (!in_symboltable(ast->token.content, st)) {
            fprintf(stderr, "line %d: variable \"%s\" not declared\n", ast->token.linenum, ast->token.content);
            exit(EXIT_FAILURE);
        }
        break;
    default:
        break;
    }
}

void ast_analyse(AST *ast, SymbolTable *st) {
    ASTKind kind = ast->kind;
    switch (kind) {
    case AST_START:
        ast_analyse_stmtlist(&(ast->is.start.stmts), st);
        break;
    case AST_ASSIGN:
        ast_analyse_expr(ast->is.assign.value, st);
        if (!in_symboltable(ast->is.assign.target->token.content, st)) {
            append_symbol(ast->is.assign.target->token, st);
        }
        break;
    case AST_IF_STMT:
        ast_analyse_expr(ast->is.if_stmt.condition, st);
        ast_analyse_stmtlist(&(ast->is.if_stmt.stmts), st);
        if (ast->is.if_stmt.else_stmt != NULL) {
            ast_analyse(ast->is.if_stmt.else_stmt, st);
        }
        break;
    case AST_ELSE_STMT:
        ast_analyse_stmtlist(&(ast->is.else_stmt.stmts), st);
        break;
    case AST_WHILE_STMT:
        ast_analyse_expr(ast->is.while_stmt.condition, st);
        ast_analyse_stmtlist(&(ast->is.while_stmt.stmts), st);
        break;
    case AST_EXPR:
        ast_analyse_expr(ast, st);
        break;
    case AST_ID:
        break;
    case AST_INT_LITERAL:
        break;
    }
}
