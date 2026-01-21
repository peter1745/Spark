#pragma once

#include <stddef.h>
#include <stdint.h>

// FIXME: Replace all individual keyword types
//        with a single "keyword" type which
//        simply matches a pre-determined
//        list of reserved keywords

#define SPK_TOKEN_TYPE(...)
#define SPK_TOKEN_ENUM_ITER() \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_CHAR, nullptr) \
    \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_IDENTIFIER, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_STRING, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_INTEGER, nullptr) \
    \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_KEYWORD, nullptr) \
    \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_EOF, nullptr)
#undef SPK_TOKEN_TYPE

#define SPK_TOKEN_TYPE(name, ...) name,
typedef enum {
    SPK_TOKEN_ENUM_ITER()
} SPK_token_type;
#undef SPK_TOKEN_TYPE

typedef struct spk_token_s {
    SPK_token_type type;
    char          *value;
    uint32_t       line;
} spk_token_t;

typedef struct spk_token_list_s {
    spk_token_t *elems;
    size_t       count;
} spk_token_list_t;

void spk_print_token (const spk_token_t token);
spk_token_list_t spk_tokenize_source (const char *src, size_t len);

