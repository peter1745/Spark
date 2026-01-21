#include "parser.h"
#include "expressions.h"
#include "printer.h"
#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct spk_parser_ctx_s {
    const spk_token_list_t *tokens;
    size_t current;
} spk_parser_ctx_t;

static bool
spk_match_any (spk_parser_ctx_t *ctx, ...)
{
    if (ctx->current >= ctx->tokens->count) {
        return false;
    }

    va_list types;
    va_start (types);
    size_t count = va_arg (types, size_t);
    for (size_t i = 0; i < count; ++i) {
        auto type = va_arg (types, SPK_token_type);

        if (ctx->tokens->elems[ctx->current].type == type) {
            ctx->current++;
            return true;
        }
    }

    return false;
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
    size_t prev = ctx->current;
    if (spk_match_any (ctx, 2, SPK_TOKEN_TYPE_INTEGER, SPK_TOKEN_TYPE_STRING)) {
        auto value = ctx->tokens->elems[prev];
        auto expr = spk_alloc_expr (SPK_EXPR_TYPE_LITERAL);
        expr->literal = (spk_literal_expr_t) {
            .value = value.literal
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

    return nullptr;
}

static spk_expr_t *
spk_unary (spk_parser_ctx_t *ctx)
{
    size_t prev = ctx->current;
    if (spk_match_any (ctx, 2, SPK_TOKEN_TYPE_NOT, SPK_TOKEN_TYPE_MINUS)) {
        auto operator = ctx->tokens->elems[prev];
        auto right = spk_unary (ctx);
        auto expr = spk_alloc_expr (SPK_EXPR_TYPE_UNARY);
        expr->unary = (spk_unary_expr_t) {
            .operator = operator,
            .right = right
        };
    }

    return spk_primary (ctx);
}

static spk_expr_t *
spk_factor (spk_parser_ctx_t *ctx)
{
    auto expr = spk_unary (ctx);

    size_t prev = ctx->current;
    while (spk_match_any (ctx, 2, SPK_TOKEN_TYPE_DIVIDE,
                                  SPK_TOKEN_TYPE_MULTIPLY)) {
        auto operator = ctx->tokens->elems[prev];
        auto left = expr;
        auto right = spk_unary (ctx);
        expr = spk_alloc_expr (SPK_EXPR_TYPE_BINARY);
        expr->binary = (spk_binary_expr_t) {
            .left = left,
            .operator = operator,
            .right = right
        };
    }

    return expr;
}

static spk_expr_t *
spk_term (spk_parser_ctx_t *ctx)
{
    auto expr = spk_factor (ctx);

    size_t prev = ctx->current;
    while (spk_match_any (ctx, 2, SPK_TOKEN_TYPE_MINUS,
                                  SPK_TOKEN_TYPE_PLUS)) {
        auto operator = ctx->tokens->elems[prev];
        auto left = expr;
        auto right = spk_factor (ctx);
        expr = spk_alloc_expr (SPK_EXPR_TYPE_BINARY);
        expr->binary = (spk_binary_expr_t) {
            .left = left,
            .operator = operator,
            .right = right
        };
    }

    return expr;
}

static spk_expr_t *
spk_comparison (spk_parser_ctx_t *ctx)
{
    auto expr = spk_term (ctx);

    size_t prev = ctx->current;
    while (spk_match_any (ctx, 4, SPK_TOKEN_TYPE_GREATER,
                                  SPK_TOKEN_TYPE_GREATER_EQUAL,
                                  SPK_TOKEN_TYPE_LESS,
                                  SPK_TOKEN_TYPE_LESS_EQUAL)) {
        auto operator = ctx->tokens->elems[prev];
        auto left = expr;
        auto right = spk_term (ctx);
        expr = spk_alloc_expr (SPK_EXPR_TYPE_BINARY);
        expr->binary = (spk_binary_expr_t) {
            .left = left,
            .operator = operator,
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
        auto operator = ctx->tokens->elems[ctx->current - 1];
        auto left = expr;
        auto right = spk_comparison (ctx);
        expr = spk_alloc_expr (SPK_EXPR_TYPE_BINARY);
        expr->binary = (spk_binary_expr_t) {
            .left = left,
            .operator = operator,
            .right = right
        };
    }

    return expr;
}

static spk_expr_t *
spk_expression (spk_parser_ctx_t *ctx)
{
    return spk_equality (ctx);
}

void
spk_parser_recursive_descent (const spk_token_list_t *tokens)
{
    spk_parser_ctx_t ctx = {
        .tokens = tokens
    };

    spk_expr_t *expr = nullptr;
    do {
        expr = spk_expression (&ctx);

        if (expr) {
            spk_print_expression (expr);
        }

        ctx.current++;
    } while (ctx.current < ctx.tokens->count);
}

