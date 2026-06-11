#include "compiler.h"
#include <stdio.h>

Token tokenconsume(TokenStream *ts) {
    return ts->tokens[ts->i++];
}

Token tokenpeek(TokenStream *ts) {
    return ts->tokens[ts->i];
}

Token tokenexpect(TokenStream *ts, Type type, int value, char *expected) {
    Token token = tokenpeek(ts);
    bool matches = (token.type == type);
    if (matches) {
        switch (type) {
        case KEYWORD:
            matches = (token.is.KEYWORD.value == value);
            break;
        case SEPARATOR:
            matches = (token.is.SEPARATOR.value == value);
            break;
        case OPERATOR:
            matches = (token.is.OPERATOR.value == value);
            break;
        default:
            break;
        }
    }
    if (!matches) {
        fprintf(stderr, "line %d: expected \"%s\", got \"%s\"\n", token.linenum, expected, token.content);
        exit(EXIT_FAILURE);
    }

    return tokenconsume(ts);
}

void append_stmt(StmtList *s, AST *stmt) {
    if (s->len == s->capacity) {
        s->capacity = s->capacity == 0 ? 8 : s->capacity * 2;
        s->stmtarray = realloc(s->stmtarray, (s->capacity) * sizeof(AST *));
    }
    s->len += 1;
    s->stmtarray[s->len - 1] = stmt;
}

AST *ast_start() {
    AST *startnode = malloc(sizeof(AST));
    startnode->kind = AST_START;
    startnode->is.start.stmts.stmtarray = NULL;
    startnode->is.start.stmts.capacity = 0;
    startnode->is.start.stmts.len = 0;
    return startnode;
}

AST *node_id(TokenStream *ts) {
    AST *node = malloc(sizeof(AST));
    node->kind = AST_ID;
    node->token = tokenconsume(ts);
    return node;
}

AST *node_literal(TokenStream *ts) {
    AST *node = malloc(sizeof(AST));
    node->kind = AST_LITERAL;
    node->token = tokenconsume(ts);
    return node;
}

AST *subtree_bin(Op op, AST *left, AST *right) {
    AST *binexpr = malloc(sizeof(AST));
    binexpr->kind = AST_EXPR;
    binexpr->is.bin.op = op;
    binexpr->is.bin.left = left;
    binexpr->is.bin.right = right;
    return binexpr;
}

AST *subtree_assign(AST *target, AST *value) {
    AST *assign_node = malloc(sizeof(AST));
    assign_node->kind = AST_ASSIGN;
    assign_node->is.assign.target = target;
    assign_node->is.assign.value = value;
    return assign_node;
}

AST *parse_atom(TokenStream *ts) {
    AST *atom;
    if (tokenpeek(ts).type == ID) {
        atom = node_id(ts);
    } else if (tokenpeek(ts).type == INT_LITERAL) {
        atom = node_literal(ts);
    } else {
        fprintf(stderr, "line %d: expected identifier, got %s\n", tokenpeek(ts).linenum, tokenpeek(ts).content);
        exit(EXIT_FAILURE);
    }
    return atom;
}

AST *subtree_if_stmt(AST *condition, StmtList stmts, AST *else_stmt) {
    AST *if_stmt = malloc(sizeof(AST));
    if_stmt->kind = AST_IF_STMT;
    if_stmt->is.if_stmt.condition = condition;
    if_stmt->is.if_stmt.stmts = stmts;
    if_stmt->is.if_stmt.else_stmt = else_stmt;
    return if_stmt;
}

AST *subtree_else_stmt(StmtList stmts) {
    AST *else_stmt = malloc(sizeof(AST));
    else_stmt->kind = AST_ELSE_STMT;
    else_stmt->is.else_stmt.stmts = stmts;
    return else_stmt;
}

AST *subtree_while_stmt(AST *condition, StmtList stmts) {
    AST *while_stmt = malloc(sizeof(AST));
    while_stmt->kind = AST_WHILE_STMT;
    while_stmt->is.while_stmt.condition = condition;
    while_stmt->is.while_stmt.stmts = stmts;
    return while_stmt;
}

AST *parse_aexpr(TokenStream *ts) {
    AST *left = parse_atom(ts);
    while (tokenpeek(ts).type == OPERATOR && (tokenpeek(ts).is.OPERATOR.value == PLUS || tokenpeek(ts).is.OPERATOR.value == MINUS)) {
        Op op = tokenconsume(ts).is.OPERATOR.value;
        AST *right = parse_atom(ts);
        left = subtree_bin(op, left, right);
    }
    return left;
}

AST *parse_rexpr(TokenStream *ts) {
    AST *left = parse_aexpr(ts);
    Op op;
    if (tokenpeek(ts).type == OPERATOR && (tokenpeek(ts).is.OPERATOR.value == LESS_THAN || tokenpeek(ts).is.OPERATOR.value == GRTR_THAN || tokenpeek(ts).is.OPERATOR.value == EQUALSEQUALS)) {
        op = tokenconsume(ts).is.OPERATOR.value; //("<" | ">" | "==")
    } else {
        fprintf(stderr, "line %d: expected \"%s\", got \"%s\"\n", tokenpeek(ts).linenum, "< | > | ==", tokenpeek(ts).content);
        exit(EXIT_FAILURE);
    }
    AST *right = parse_aexpr(ts);
    left = subtree_bin(op, left, right);
    return left;
}

AST *parse_else_stmt(TokenStream *ts) {
    tokenexpect(ts, KEYWORD, ELSE, "else");
    tokenexpect(ts, SEPARATOR, LCURLY, "{");
    StmtList stmtlist = (StmtList){NULL, 0, 0};
    while (tokenpeek(ts).type != SEPARATOR || tokenpeek(ts).is.SEPARATOR.value != RCURLY) {
        AST *stmt = parse_stmt(ts);
        append_stmt(&stmtlist, stmt);
    }
    tokenexpect(ts, SEPARATOR, RCURLY, "}");
    return subtree_else_stmt(stmtlist);
}

AST *parse_assign(TokenStream *ts) {
    AST *target = node_id(ts);
    tokenexpect(ts, OPERATOR, EQUALS, "=");
    AST *value = parse_aexpr(ts);
    tokenexpect(ts, SEPARATOR, SEMI, ";");
    return subtree_assign(target, value);
}

AST *parse_if_stmt(TokenStream *ts) {
    tokenexpect(ts, KEYWORD, IF, "if");
    tokenexpect(ts, SEPARATOR, LPAREN, "(");
    AST *condition = parse_rexpr(ts);
    tokenexpect(ts, SEPARATOR, RPAREN, ")");
    tokenexpect(ts, SEPARATOR, LCURLY, "{");
    StmtList stmtlist = (StmtList){NULL, 0, 0};
    while (tokenpeek(ts).type != SEPARATOR || tokenpeek(ts).is.SEPARATOR.value != RCURLY) {
        AST *stmt = parse_stmt(ts);
        append_stmt(&stmtlist, stmt);
    }
    tokenexpect(ts, SEPARATOR, RCURLY, "}");
    AST *else_stmt;
    if (tokenpeek(ts).type == KEYWORD && tokenpeek(ts).is.KEYWORD.value == ELSE) {
        else_stmt = parse_else_stmt(ts);
    } else {
        else_stmt = NULL;
    }
    return subtree_if_stmt(condition, stmtlist, else_stmt);
}

AST *parse_while_stmt(TokenStream *ts) {
    tokenexpect(ts, KEYWORD, WHILE, "while");
    tokenexpect(ts, SEPARATOR, LPAREN, "(");
    AST *condition = parse_rexpr(ts);
    tokenexpect(ts, SEPARATOR, RPAREN, ")");
    tokenexpect(ts, SEPARATOR, LCURLY, "{");
    StmtList stmtlist = (StmtList){NULL, 0, 0};
    while (tokenpeek(ts).type != SEPARATOR || tokenpeek(ts).is.SEPARATOR.value != RCURLY) {
        AST *stmt = parse_stmt(ts);
        append_stmt(&stmtlist, stmt);
    }
    tokenexpect(ts, SEPARATOR, RCURLY, "}");
    return subtree_while_stmt(condition, stmtlist);
}

AST *parse_stmt(TokenStream *ts) {
    AST *stmt;
    if (tokenpeek(ts).type == ID) {
        stmt = parse_assign(ts);
    } else if (tokenpeek(ts).type == KEYWORD) {
        if (tokenpeek(ts).is.KEYWORD.value == IF) {
            stmt = parse_if_stmt(ts);
        } else if (tokenpeek(ts).is.KEYWORD.value == WHILE) {
            stmt = parse_while_stmt(ts);
        } else {
            fprintf(stderr, "line %d: expected statement\n", tokenpeek(ts).linenum);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "line %d: expected statement\n", tokenpeek(ts).linenum);
        exit(EXIT_FAILURE);
    }
    return stmt;
}

AST *start_parse(TokenStream *ts) {
    AST *startnode = ast_start();
    while (tokenpeek(ts).type != TOK_EOF) {
        AST *stmt = parse_stmt(ts);
        append_stmt(&(startnode->is.start.stmts), stmt);
    }
    return startnode;
}
