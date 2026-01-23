#include "ast_interpreter.h"
#include "expressions.h"
#include "statements.h"

#include "../utils/darray.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef struct spk_var_storage_s {
    const char *name;
    spk_token_literal_t value;
} spk_var_storage_t;

typedef struct spk_environment_s {
    darray_t *var_storage; // [spk_var_storage_t, ...]
} spk_environment_t;

static spk_environment_t global_env = {
    .var_storage = nullptr
};

static spk_token_literal_t *
spk_global_reg_find_var (const char *name)
{
    // FIXME: Linear search = bad
    for (size_t i = 0; i < global_env.var_storage->count; ++i) {
        spk_var_storage_t *storage = darray_elem (global_env.var_storage, i);
        if (strcmp (storage->name, name) == 0) {
            return &storage->value;
        }
    }

    return nullptr;
}

static void
spk_global_reg_add_var (const spk_var_statement_t *var)
{
    if (!global_env.var_storage) {
        global_env.var_storage = darray_empty (sizeof (spk_var_storage_t));
    }

    if (spk_global_reg_find_var (var->name.value)) {
        // Variable already declared
        assert (false);
        return;
    }

    spk_var_storage_t storage = {
        .name = strdup (var->name.value),
        .value = {
            .type = SPK_TOKEN_LITERAL_EMPTY
        }
    };

    if (var->initializer) {
        storage.value = spk_evaluate_expression (var->initializer);
    }

    darray_append (global_env.var_storage, &storage);
}

static spk_token_literal_t
spk_evaluate_literal (const spk_literal_expr_t *expr)
{
    return expr->value;
}

static spk_token_literal_t
spk_evaluate_grouping (const spk_grouping_expr_t *expr)
{
    return spk_evaluate_expression (expr->expr);
}

static spk_token_literal_t
spk_evaluate_unary (const spk_unary_expr_t *expr)
{
    auto right = spk_evaluate_expression (expr->right);

    if (expr->operator.type == SPK_TOKEN_TYPE_MINUS) {
        return (spk_token_literal_t) {
            .type = SPK_TOKEN_LITERAL_INTEGER,
            .integer = { -right.integer.value }
        };
    }

    return (spk_token_literal_t) {};
}

static spk_token_literal_t
spk_evaluate_binary (const spk_binary_expr_t *expr)
{
    auto left = spk_evaluate_expression (expr->left);
    auto right = spk_evaluate_expression (expr->right);
 
    assert (left.type == SPK_TOKEN_LITERAL_INTEGER);
    assert (right.type == SPK_TOKEN_LITERAL_INTEGER);

    int32_t result;
    switch (expr->operator.type) {
        case SPK_TOKEN_TYPE_PLUS:
            result = left.integer.value + right.integer.value;
            break;
        case SPK_TOKEN_TYPE_MINUS:
            result = left.integer.value - right.integer.value;
            break;
        case SPK_TOKEN_TYPE_MULTIPLY:
            result = left.integer.value * right.integer.value;
            break;
        case SPK_TOKEN_TYPE_DIVIDE:
            result = left.integer.value / right.integer.value;
            break;
        case SPK_TOKEN_TYPE_GREATER:
            result = left.integer.value > right.integer.value;
            break;
        case SPK_TOKEN_TYPE_GREATER_EQUAL:
            result = left.integer.value >= right.integer.value;
            break;
        case SPK_TOKEN_TYPE_LESS:
            result = left.integer.value < right.integer.value;
            break;
        case SPK_TOKEN_TYPE_LESS_EQUAL:
            result = left.integer.value <= right.integer.value;
            break;
        case SPK_TOKEN_TYPE_EQUAL_EQUAL:
            result = left.integer.value == right.integer.value;
            break;
        case SPK_TOKEN_TYPE_NOT_EQUAL:
            result = left.integer.value != right.integer.value;
            break;
        default:
            return (spk_token_literal_t) {};
    }

    return (spk_token_literal_t) {
        .type = SPK_TOKEN_LITERAL_INTEGER,
        .integer = { result }
    };
}

spk_token_literal_t
spk_evaluate_expression (const spk_expr_t *expr)
{
    spk_token_literal_t evaluated_value = {
        .type = SPK_TOKEN_LITERAL_EMPTY
    };

    switch (expr->type) {
        case SPK_EXPR_TYPE_LITERAL:
            evaluated_value = spk_evaluate_literal (&expr->literal);
            break;
        case SPK_EXPR_TYPE_GROUPING:
            evaluated_value = spk_evaluate_grouping (&expr->grouping);
            break;
        case SPK_EXPR_TYPE_UNARY:
            evaluated_value = spk_evaluate_unary (&expr->unary);
            break;
        case SPK_EXPR_TYPE_BINARY:
            evaluated_value = spk_evaluate_binary (&expr->binary);
            break;
        case SPK_EXPR_TYPE_VAR: {
            auto var_storage = spk_global_reg_find_var (expr->var.name.value);

            if (!var_storage) {
                printf ("Error trying to evaluate unknown variable %s!\n", expr->var.name.value);
                break;
            }

            evaluated_value = *var_storage;
            break;
        }
        case SPK_EXPR_TYPE_ASSIGNMENT: {
            auto var_storage = spk_global_reg_find_var (expr->assignment.name.value);

            if (!var_storage) {
                printf ("Error trying to assign to an unknown variable %s!\n", expr->assignment.name.value);
                break;
            }

            evaluated_value = spk_evaluate_expression (expr->assignment.rhs);
            *var_storage = evaluated_value;
            break;
        }
        default:
            assert (false);
    }

    return evaluated_value;
}

void
spk_interpret_statement (const spk_statement_t *stmt)
{
    switch (stmt->type) {
        case SPK_STATEMENT_TYPE_PRINT:
            auto msg = spk_evaluate_expression (stmt->print.expr);
            switch (msg.type) {
                case SPK_TOKEN_LITERAL_EMPTY:
                    printf ("empty\n");
                    break;
                case SPK_TOKEN_LITERAL_INTEGER:
                    printf ("%d\n", msg.integer.value);
                    break;
                case SPK_TOKEN_LITERAL_STRING:
                    printf ("%s\n", msg.string.value);
                    break;
                default:
                    assert (false);
            }

            break;
        case SPK_STATEMENT_TYPE_EXPR:
            auto result = spk_evaluate_expression (stmt->expr.expr);
#if 0
            switch (result.type) {
                case SPK_TOKEN_LITERAL_INTEGER:
                    printf ("Evaluated to %d\n", result.integer.value);
                    break;
                case SPK_TOKEN_LITERAL_STRING:
                    printf ("Evaluated to %s\n", result.string.value);
                    break;
                default:
                    printf ("Couldn't evaluate. Invalid expression?\n");
                    break;
            }
#else
            (void)result;
#endif
            break;
        case SPK_STATEMENT_TYPE_VAR:
            //printf ("Declaring variable %s", stmt->var.name.value);
            spk_global_reg_add_var (&stmt->var);

            /*if (stmt->var.initializer) {
                printf (" with value");
                auto value = spk_evaluate_expression(stmt->var.initializer);
                switch (value.type) {
                    case SPK_TOKEN_LITERAL_INTEGER:
                        printf (" %d\n", value.integer.value);
                        break;
                    case SPK_TOKEN_LITERAL_STRING:
                        printf (" \"%s\"\n", value.string.value);
                        break;
                    default:
                        assert (false);
                }
            } else {
                printf ("\n");
            }*/
            break;
        default:
            assert (false);
    }
}

