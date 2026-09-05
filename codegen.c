#include "compiler.h"
const char *FMT_INT = "\"%d\\n\"";
static int label = 0;

int new_label(void) {
    label++;
    return (label - 1);
}

void gen_stmtlist(FILE *fp, StmtList *list, SymbolTable *st) {
    for (int i = 0; i < list->len; i++) {
        ast_gen(fp, list->stmtarray[i], st);
    }
}

void ast_gen(FILE *fp, AST *ast, SymbolTable *st) {
    ASTKind kind = ast->kind;
    switch (kind) {
    case AST_START: {
        fprintf(fp, ".section .rodata\n");
        fprintf(fp, ".LC0:\n");
        fprintf(fp, ".string %s\n", FMT_INT);

        fprintf(fp, ".bss\n");
        for (int i = 0; i < st->len; i++) {
            fprintf(fp, "var_%s: .zero 8\n", st->symbols[i]);
        }

        fprintf(fp, ".text\n");
        fprintf(fp, ".globl main\n");
        fprintf(fp, "main:\n");

        fprintf(fp, "pushq %%rbp\n");
        fprintf(fp, "movq %%rsp, %%rbp\n");

        gen_stmtlist(fp, &(ast->is.start.stmts), st);

        fprintf(fp, "movl $0, %%eax\n");
        fprintf(fp, "popq %%rbp\n");
        fprintf(fp, "ret\n");
        break;
    }
    case AST_ASSIGN: {
        ast_gen(fp, ast->is.assign.value, st);
        fprintf(fp, "popq %%rax\n");
        fprintf(fp, "movq %%rax, var_%s(%%rip)\n", ast->is.assign.target->token.content);
        break;
    }
    case AST_IF_STMT: {
        int label_if = new_label(), label_exit = new_label();
        ast_gen(fp, ast->is.if_stmt.condition, st);

        fprintf(fp, "popq %%rax\n");
        fprintf(fp, "cmpq $0, %%rax\n");
        fprintf(fp, "je .L%d\n", label_if);

        gen_stmtlist(fp, &(ast->is.if_stmt.stmts), st);

        fprintf(fp, "jmp .L%d\n", label_exit);
        fprintf(fp, ".L%d:\n", label_if);

        if (ast->is.if_stmt.else_stmt != NULL) {
            gen_stmtlist(fp, &(ast->is.if_stmt.else_stmt->is.else_stmt.stmts), st);
        }
        fprintf(fp, ".L%d:\n", label_exit);
        break;
    }
    case AST_ELSE_STMT: {
        break;
    }
    case AST_WHILE_STMT: {
        int label_exit = new_label(), label_top = new_label();

        fprintf(fp, ".L%d:\n", label_top);
        ast_gen(fp, ast->is.while_stmt.condition, st);
        fprintf(fp, "popq %%rax\n");
        fprintf(fp, "cmpq $0, %%rax\n");
        fprintf(fp, "je .L%d\n", label_exit);

        gen_stmtlist(fp, &(ast->is.while_stmt.stmts), st);

        fprintf(fp, "jmp .L%d\n", label_top);
        fprintf(fp, ".L%d:\n", label_exit);
        break;
    }
    case AST_EXPR: {
        ast_gen(fp, ast->is.bin.left, st);
        ast_gen(fp, ast->is.bin.right, st);
        fprintf(fp, "popq %%rcx\n");
        fprintf(fp, "popq %%rax\n");
        switch (ast->is.bin.op) {
        case PLUS:
            fprintf(fp, "addq %%rcx, %%rax\n");
            fprintf(fp, "pushq %%rax\n");
            break;
        case MINUS:
            fprintf(fp, "subq %%rcx, %%rax\n");
            fprintf(fp, "pushq %%rax\n");
            break;
        case LESS_THAN:
            fprintf(fp, "cmpq %%rcx, %%rax\n");
            fprintf(fp, "movl $0, %%eax\n");
            fprintf(fp, "setl %%al\n");
            fprintf(fp, "pushq %%rax\n");
            break;
        case GRTR_THAN:
            fprintf(fp, "cmpq %%rcx, %%rax\n");
            fprintf(fp, "movl $0, %%eax\n");
            fprintf(fp, "setg %%al\n");
            fprintf(fp, "pushq %%rax\n");
            break;
        case EQUALSEQUALS:
            fprintf(fp, "cmpq %%rcx, %%rax\n");
            fprintf(fp, "movl $0, %%eax\n");
            fprintf(fp, "sete %%al\n");
            fprintf(fp, "pushq %%rax\n");
            break;
        default:
            break;
        }
        break;
    }
    case AST_OUTPUT: {
        ast_gen(fp, ast->is.output.aexpr, st);
        fprintf(fp, "popq %%rsi\n");
        fprintf(fp, "leaq .LC0(%%rip), %%rdi\n");
        fprintf(fp, "movl $0, %%eax\n");
        fprintf(fp, "call printf@PLT\n");
        break;
    }
    case AST_ID: {
        fprintf(fp, "pushq var_%s(%%rip)\n", ast->token.content);
        break;
    }
    case AST_INT_LITERAL: {
        fprintf(fp, "pushq $%s\n", ast->token.content);
        break;
    }
    }
}
