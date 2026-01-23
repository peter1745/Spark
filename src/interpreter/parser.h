#pragma once

#include "../utils/darray.h"

typedef darray_t *spk_token_list_t;
darray_t *spk_parser_recursive_descent (const spk_token_list_t tokens);

