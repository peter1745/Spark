#pragma once

#include <stddef.h>

typedef struct spk_token_s spk_token_t;
typedef struct spk_token_list_s {
    spk_token_t *elems;
    size_t       count;
} spk_token_list_t;

spk_token_list_t spk_tokenize_source (const char *src, size_t len);
void spk_free_token_list (spk_token_list_t *list);

