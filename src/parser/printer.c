#include "printer.h"
#include "expressions.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static char *
spk_process_expression (const spk_expr_t *expr);

static char *
spk_parenthesize_value (const char *name, ...)
{
    size_t name_len = strlen (name);
    size_t off = 0;
    size_t len = name_len + 2;

    // TODO: Dynamic strings + better allocation strategy
    char *buf = calloc (len, sizeof (char));

    buf[off++] = '(';
    memcpy (buf + off, name, name_len);
    off += name_len;

    va_list exprs;
    va_start (exprs);
    int32_t num_exprs = va_arg (exprs, int32_t);
    for (int32_t i = 0; i < num_exprs; ++i) {
        auto expr = va_arg (exprs, spk_expr_t*);
        auto str = spk_process_expression (expr);
        size_t str_len = strlen (str);

        // Ensure buf is large enough to hold str + space
        if (off + str_len >= len) {
            auto min_len = len + str_len + 1;
            // Grow more than needed to reduce number of allocations
            char *tmp = calloc (min_len * 2, sizeof (char));
            memcpy (tmp, buf, len * sizeof (char));
            free (buf);
            buf = tmp;
            len = min_len * 2;
        }

        buf[off++] = ' ';
        memcpy (buf + off, str, str_len * sizeof (char));
        off += str_len;
    }
    va_end (exprs);

    size_t actual_len = strlen (buf);
    // +2 to make sure we can fit ')' and null-terminator
    char *res = calloc (actual_len + 2, sizeof (char));
    memcpy (res, buf, actual_len * sizeof (char));
    free (buf);
    res[actual_len] = ')';
    return res;
}

static char *
spk_process_literal (const spk_literal_expr_t *expr)
{
    if (!expr->value) {
        return "nil";
    }

    return expr->value;
}

static char *
spk_process_grouping (const spk_grouping_expr_t *expr)
{
    return spk_parenthesize_value ("group", 1, expr->expr);
}

static char *
spk_process_unary (const spk_unary_expr_t *expr)
{
    return spk_parenthesize_value(expr->operator.value, 1, expr->right);
}

static char *
spk_process_binary (const spk_binary_expr_t *expr)
{
    return spk_parenthesize_value(expr->operator.value, 2, expr->left, expr->right);
}

static char *
spk_process_expression (const spk_expr_t *expr)
{
    char *res = nullptr;
    switch (expr->type) {
        case SPK_EXPR_TYPE_LITERAL:
            res = spk_process_literal (&expr->literal);
            break;
        case SPK_EXPR_TYPE_GROUPING:
            res = spk_process_grouping (&expr->grouping);
            break;
        case SPK_EXPR_TYPE_UNARY:
            res = spk_process_unary (&expr->unary);
            break;
        case SPK_EXPR_TYPE_BINARY:
            res = spk_process_binary (&expr->binary);
            break;
    }

    return res;
}

static const char *
spk_expression_type_str (SPK_expr_type type)
{
    switch (type) {
        case SPK_EXPR_TYPE_LITERAL: return "SPK_EXPR_TYPE_LITERAL";
        case SPK_EXPR_TYPE_UNARY: return "SPK_EXPR_TYPE_UNARY";
        case SPK_EXPR_TYPE_GROUPING: return "SPK_EXPR_TYPE_GROUPING";
        case SPK_EXPR_TYPE_BINARY: return "SPK_EXPR_TYPE_BINARY";
        default: return "Unknown";
    }
}

void spk_print_expression (const spk_expr_t *expr)
{
    char *expr_str = spk_process_expression (expr);
    printf ("Expression:\n");
    printf ("\tType: %s\n", spk_expression_type_str (expr->type));
    printf ("\t%s\n", expr_str);
    free (expr_str);
}

