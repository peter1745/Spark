#pragma once

#include "expressions.h"

typedef struct spk_expr_statement_s {
    spk_expr_t *expr;
} spk_expr_statement_t;

typedef struct spk_print_statement_s {
    spk_expr_t *expr;
} spk_print_statement_t;

typedef enum {
    SPK_STATEMENT_TYPE_EMPTY,
    SPK_STATEMENT_TYPE_EXPR,
    SPK_STATEMENT_TYPE_PRINT,
} SPK_statement_type;

typedef struct spk_statement_s {
    SPK_statement_type type;
    union {
        spk_expr_statement_t expr;
        spk_print_statement_t print;
    };
} spk_statement_t;

