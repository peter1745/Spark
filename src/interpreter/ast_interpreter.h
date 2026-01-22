#pragma once

#include "token.h"

typedef struct spk_expr_s spk_expr_t;
spk_token_literal_t spk_evaluate_expression (const spk_expr_t *expr);

