#pragma once

#include "../utils/darray.h"

#include <stddef.h>

typedef struct spk_token_s spk_token_t;
typedef darray_t *spk_token_list_t;

spk_token_list_t spk_tokenize_source (const char *src, size_t len);

