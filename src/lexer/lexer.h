#pragma once

#include <stddef.h>
#include <stdint.h>

// TODO: Consider if the lexer should be more intricate
//       about the grammar... E.g should we define operator
//       and keyword tokens explicitly? Advantage of early-out
//       in case of syntax errors?

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
void spk_free_token_list (spk_token_list_t *list);

