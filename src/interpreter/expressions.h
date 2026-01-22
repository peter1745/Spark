#pragma once

#include "token.h"

/* grammar/expr.ebnf */

typedef struct spk_expr_s spk_expr_t;

typedef struct spk_literal_expr_s {
    spk_token_literal_t value;
} spk_literal_expr_t;

typedef struct spk_grouping_expr_s {
    spk_expr_t *expr;
} spk_grouping_expr_t;

typedef struct spk_unary_expr_s {
    spk_token_t operator;
    spk_expr_t  *right;
} spk_unary_expr_t;

typedef struct spk_binary_expr_s {
    spk_expr_t  *left;
    spk_token_t operator;
    spk_expr_t  *right;
} spk_binary_expr_t;

typedef struct spk_var_expr_s {
    spk_token_t name;
} spk_var_expr_t;

typedef enum {
    SPK_EXPR_TYPE_LITERAL,
    SPK_EXPR_TYPE_GROUPING,
    SPK_EXPR_TYPE_UNARY,
    SPK_EXPR_TYPE_BINARY,
    SPK_EXPR_TYPE_VAR,
} SPK_expr_type;

typedef struct spk_expr_s {
    SPK_expr_type type;
    union {
        spk_literal_expr_t  literal;
        spk_grouping_expr_t grouping;
        spk_unary_expr_t    unary;
        spk_binary_expr_t   binary;
        spk_var_expr_t      var;
    };
} spk_expr_t;

