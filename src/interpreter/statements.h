#pragma once

#include "expressions.h"

typedef struct spk_expr_statement_s {
    spk_expr_t *expr;
} spk_expr_statement_t;

typedef struct spk_print_statement_s {
    spk_expr_t *expr;
} spk_print_statement_t;

typedef struct spk_var_statement_s {
    spk_token_t name;
    spk_expr_t *initializer;
    bool mutable;
} spk_var_statement_t;

typedef enum {
    SPK_STATEMENT_TYPE_EMPTY,
    SPK_STATEMENT_TYPE_EXPR,
    SPK_STATEMENT_TYPE_PRINT,
    SPK_STATEMENT_TYPE_VAR,
} SPK_statement_type;

typedef struct spk_statement_s {
    SPK_statement_type type;
    union {
        spk_expr_statement_t expr;
        spk_print_statement_t print;
        spk_var_statement_t var;
    };
} spk_statement_t;

