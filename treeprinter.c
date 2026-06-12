#include "compiler.h"
char *oplist[6] = {"+", "-", "=", "<", ">", "=="};

void ast_print(AST *ast) {
    ASTKind kind = ast->kind;
    switch (kind) {
    case AST_START:
        printf("( Start \n");
        for (int i = 0; i < ast->is.start.stmts.len; i++) {
            ast_print(ast->is.start.stmts.stmtarray[i]);
            printf("\n");
        }
        printf(")");
        break;
    case AST_ASSIGN:
        printf("( =");
        ast_print(ast->is.assign.target);
        ast_print(ast->is.assign.value);
        printf(")");
        break;
    case AST_IF_STMT:
        printf("( if");
        ast_print(ast->is.if_stmt.condition);
        for (int i = 0; i < ast->is.if_stmt.stmts.len; i++) {
            ast_print(ast->is.if_stmt.stmts.stmtarray[i]);
        }
        if (ast->is.if_stmt.else_stmt != NULL) {
            ast_print(ast->is.if_stmt.else_stmt);
        }
        printf(")");
        break;
    case AST_ELSE_STMT:
        printf("( else");
        for (int i = 0; i < ast->is.else_stmt.stmts.len; i++) {
            ast_print(ast->is.else_stmt.stmts.stmtarray[i]);
        }
        printf(")");
        break;
    case AST_WHILE_STMT:
        printf("( while");
        ast_print(ast->is.while_stmt.condition);
        for (int i = 0; i < ast->is.while_stmt.stmts.len; i++) {
            ast_print(ast->is.while_stmt.stmts.stmtarray[i]);
        }
        printf(")");
        break;
    case AST_EXPR:
        printf("( %s", oplist[ast->is.bin.op]);
        ast_print(ast->is.bin.left);
        ast_print(ast->is.bin.right);
        printf(")");
        break;
    case AST_ID:
        printf(" %s", ast->token.content);
        break;
    case AST_LITERAL:
        printf(" %s ", ast->token.content);
        break;
    }
}
