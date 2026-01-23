#include "parser.h"
#include "expressions.h"
#include "statements.h"
#include "printer.h"
#include "lexer.h"
#include "ast_interpreter.h"
#include "../utils/stacktrace.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct spk_parser_ctx_s {
    darray_t               *statements;
    const spk_token_list_t tokens;
    size_t current;
} spk_parser_ctx_t;

static bool
spk_parser_at_end (spk_parser_ctx_t *ctx)
{
    return ctx->current >= ctx->tokens->count;
}

static spk_token_t *
spk_consume (spk_parser_ctx_t *ctx, SPK_token_type type, const char *err)
{
    if (spk_parser_at_end (ctx)) {
        printf ("Reached end of file...\n");
        return nullptr;
    }

    spk_token_t *token = darray_elem(ctx->tokens, ctx->current++);
    if (!token || token->type != type) {
        printf ("Parser error: %s\n", err);
        printf ("\tExpected token type %s, was %s\n",
                spk_token_type_str (type), spk_token_type_str (token->type));

        if (ctx->current > 0) {
            spk_token_t *prev = darray_elem(ctx->tokens, ctx->current - 2);
            printf ("\tPrevious: %s\n", spk_token_type_str (prev->type));
        }

        if (ctx->current < ctx->tokens->count - 1) {
            spk_token_t *next = darray_elem(ctx->tokens, ctx->current - 2);
            printf ("\tNext: %s\n", spk_token_type_str (next->type));
        }

        spk_dump_stacktrace ();
    }
    return token;
}

static bool
spk_match_any (spk_parser_ctx_t *ctx, ...)
{
    if (spk_parser_at_end (ctx)) {
        return false;
    }

    va_list types;
    va_start (types);
    size_t count = va_arg (types, size_t);
    for (size_t i = 0; i < count; ++i) {
        auto type = va_arg (types, SPK_token_type);
        spk_token_t *token = darray_elem (ctx->tokens, ctx->current);
        if (token->type == type) {
            ctx->current++;
            return true;
        }
    }

    return false;
}

static spk_token_t *
spk_prev (spk_parser_ctx_t *ctx)
{
    return darray_elem (ctx->tokens, ctx->current - 1);
}

static spk_expr_t *
spk_alloc_expr (SPK_expr_type type)
{
    spk_expr_t *expr = calloc (1, sizeof (spk_expr_t));
    expr->type = type;
    return expr;
}

static spk_expr_t *
spk_expression (spk_parser_ctx_t *ctx);

static spk_expr_t *
spk_primary (spk_parser_ctx_t *ctx)
{
    if (spk_match_any (ctx, 2, SPK_TOKEN_TYPE_INTEGER, SPK_TOKEN_TYPE_STRING)) {
        auto value = spk_prev (ctx);
        auto expr = spk_alloc_expr (SPK_EXPR_TYPE_LITERAL);
        expr->literal = (spk_literal_expr_t) {
            .value = value->literal
        };
        return expr;
    }
 
    if (spk_match_any(ctx, 1, SPK_TOKEN_TYPE_LEFT_PAREN)) {
        auto expr = spk_expression (ctx);

        if (!spk_match_any(ctx, 1, SPK_TOKEN_TYPE_RIGHT_PAREN)) {
            printf ("Expected ')' after expression\n");
        }
     
        auto grouping = spk_alloc_expr (SPK_EXPR_TYPE_GROUPING);
        grouping->grouping = (spk_grouping_expr_t) { expr };
        return grouping;
    }

    if (spk_match_any (ctx, 1, SPK_TOKEN_TYPE_IDENTIFIER)) {
        auto name = spk_prev (ctx);
        auto expr = spk_alloc_expr (SPK_EXPR_TYPE_VAR);
        expr->var = (spk_var_expr_t) {
            .name = *name
        };
        return expr;
    }

    return nullptr;
}

static spk_expr_t *
spk_unary (spk_parser_ctx_t *ctx)
{
    if (spk_match_any (ctx, 2, SPK_TOKEN_TYPE_NOT, SPK_TOKEN_TYPE_MINUS)) {
        auto operator = spk_prev (ctx);
        auto right = spk_unary (ctx);
        auto expr = spk_alloc_expr (SPK_EXPR_TYPE_UNARY);
        expr->unary = (spk_unary_expr_t) {
            .operator = *operator,
            .right = right
        };
    }

    return spk_primary (ctx);
}

static spk_expr_t *
spk_factor (spk_parser_ctx_t *ctx)
{
    auto expr = spk_unary (ctx);

    while (spk_match_any (ctx, 2, SPK_TOKEN_TYPE_DIVIDE,
                                  SPK_TOKEN_TYPE_MULTIPLY)) {
        auto operator = spk_prev (ctx);
        auto left = expr;
        auto right = spk_unary (ctx);
        expr = spk_alloc_expr (SPK_EXPR_TYPE_BINARY);
        expr->binary = (spk_binary_expr_t) {
            .left = left,
            .operator = *operator,
            .right = right
        };
    }

    return expr;
}

static spk_expr_t *
spk_term (spk_parser_ctx_t *ctx)
{
    auto expr = spk_factor (ctx);

    while (spk_match_any (ctx, 2, SPK_TOKEN_TYPE_MINUS,
                                  SPK_TOKEN_TYPE_PLUS)) {
        auto operator = spk_prev (ctx);
        auto left = expr;
        auto right = spk_factor (ctx);
        expr = spk_alloc_expr (SPK_EXPR_TYPE_BINARY);
        expr->binary = (spk_binary_expr_t) {
            .left = left,
            .operator = *operator,
            .right = right
        };
    }

    return expr;
}

static spk_expr_t *
spk_comparison (spk_parser_ctx_t *ctx)
{
    auto expr = spk_term (ctx);

    while (spk_match_any (ctx, 4, SPK_TOKEN_TYPE_GREATER,
                                  SPK_TOKEN_TYPE_GREATER_EQUAL,
                                  SPK_TOKEN_TYPE_LESS,
                                  SPK_TOKEN_TYPE_LESS_EQUAL)) {
        auto operator = spk_prev (ctx);
        auto left = expr;
        auto right = spk_term (ctx);
        expr = spk_alloc_expr (SPK_EXPR_TYPE_BINARY);
        expr->binary = (spk_binary_expr_t) {
            .left = left,
            .operator = *operator,
            .right = right
        };
    }

    return expr;
}

static spk_expr_t *
spk_equality (spk_parser_ctx_t *ctx)
{
    auto expr = spk_comparison (ctx);

    while (spk_match_any (ctx, 2, SPK_TOKEN_TYPE_NOT_EQUAL,
                                  SPK_TOKEN_TYPE_EQUAL_EQUAL)) {
        auto operator = spk_prev (ctx);
        auto left = expr;
        auto right = spk_comparison (ctx);
        expr = spk_alloc_expr (SPK_EXPR_TYPE_BINARY);
        expr->binary = (spk_binary_expr_t) {
            .left = left,
            .operator = *operator,
            .right = right
        };
    }

    return expr;
}

static spk_expr_t *
spk_assignment (spk_parser_ctx_t *ctx)
{
    auto expr = spk_equality (ctx);

    if (spk_match_any (ctx, 1, SPK_TOKEN_TYPE_EQUAL)) {
        auto value = spk_assignment (ctx);

        if (expr->type == SPK_EXPR_TYPE_VAR) {
            auto name = expr->var.name;
            free (expr);
            expr = spk_alloc_expr (SPK_EXPR_TYPE_ASSIGNMENT);
            expr->assignment = (spk_assignment_expr_t) {
                .name = name,
                .rhs = value
            };
        }
    }

    return expr;
}

static spk_expr_t *
spk_expression (spk_parser_ctx_t *ctx)
{
    return spk_assignment (ctx);
}

static spk_statement_t
spk_expression_statement (spk_parser_ctx_t *ctx)
{
    auto expr = spk_expression (ctx);

    if (!expr) {
        ctx->current++;
        return (spk_statement_t) { .type = SPK_STATEMENT_TYPE_EMPTY };
    }

    spk_consume(ctx, SPK_TOKEN_TYPE_SEMICOLON, "Expected ; after expression.");
    return (spk_statement_t) {
        .type = SPK_STATEMENT_TYPE_EXPR,
        .expr = {
            .expr = expr
        }
    };
}

static spk_statement_t
spk_print_statement (spk_parser_ctx_t *ctx)
{
    auto expr = spk_expression (ctx);
    spk_consume(ctx, SPK_TOKEN_TYPE_SEMICOLON, "Expected ; after print expression.");
    return (spk_statement_t) {
        .type = SPK_STATEMENT_TYPE_PRINT,
        .print = {
            .expr = expr
        }
    };
}

static spk_statement_t
spk_variable_statement (spk_parser_ctx_t *ctx)
{
    auto ident = spk_consume (ctx, SPK_TOKEN_TYPE_IDENTIFIER, "Expected identifier name after 'var'");

    spk_expr_t *expr = nullptr;
    if (spk_match_any (ctx, 1, SPK_TOKEN_TYPE_EQUAL)) {
        // Combined declaration / assignment
        expr = spk_expression (ctx);
    }

    spk_consume (ctx, SPK_TOKEN_TYPE_SEMICOLON, "Expected ';' after variable expression.");
    return (spk_statement_t) {
        .type = SPK_STATEMENT_TYPE_VAR,
        .var = {
            .name = *ident,
            .initializer = expr,
            .mutable = true
        }
    };
}

static spk_statement_t
spk_statement (spk_parser_ctx_t *ctx)
{
    if (spk_match_any (ctx, 1, SPK_TOKEN_TYPE_PRINT)) {
        return spk_print_statement (ctx);
    }

    return spk_expression_statement (ctx);
}

static spk_statement_t
spk_declaration (spk_parser_ctx_t *ctx)
{
    if (spk_match_any(ctx, 1, SPK_TOKEN_TYPE_VAR)) {
        return spk_variable_statement (ctx);
    }

    return spk_statement (ctx);
}

darray_t *
spk_parser_recursive_descent (const spk_token_list_t tokens)
{
    spk_parser_ctx_t ctx = {
        .statements = darray_empty (sizeof (spk_statement_t)),
        .tokens = tokens
    };

    while (!spk_parser_at_end (&ctx)) {
        auto statement = spk_declaration (&ctx);
        if (statement.type != SPK_STATEMENT_TYPE_EMPTY) {
            darray_append (ctx.statements, &statement);
        }
    }

    return ctx.statements;
}

