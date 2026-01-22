#pragma once

#include <stddef.h>
#include <stdint.h>

#define SPK_TOKEN_TYPE(...)
#define SPK_TOKEN_ENUM_ITER() \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_EQUAL, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_EQUAL_EQUAL, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_AND, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_OR, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_NOT, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_NOT_EQUAL, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_GREATER, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_GREATER_EQUAL, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_LESS, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_LESS_EQUAL, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_PLUS, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_MINUS, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_DIVIDE, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_MULTIPLY, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_LEFT_PAREN, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_RIGHT_PAREN, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_LEFT_BRACE, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_RIGHT_BRACE, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_SEMICOLON, nullptr) \
    \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_IDENTIFIER, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_STRING, nullptr) \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_INTEGER, nullptr) \
    \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_VAR, "var") \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_MUT, "mut") \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_PRINT, "print") \
    \
    SPK_TOKEN_TYPE(SPK_TOKEN_TYPE_EOF, nullptr)
#undef SPK_TOKEN_TYPE

#define SPK_TOKEN_TYPE(name, ...) name,
typedef enum {
    SPK_TOKEN_ENUM_ITER()
} SPK_token_type;
#undef SPK_TOKEN_TYPE

typedef enum {
    SPK_TOKEN_LITERAL_EMPTY,
    SPK_TOKEN_LITERAL_STRING,
    SPK_TOKEN_LITERAL_INTEGER
} SPK_token_literal_type;

typedef struct spk_token_string_literal_s {
    char *value;
} spk_token_string_literal_t;

typedef struct spk_token_integer_literal_s {
    int32_t value;
} spk_token_integer_literal_t;

typedef struct spk_token_literal_s {
    SPK_token_literal_type type;
    union {
        spk_token_string_literal_t  string;
        spk_token_integer_literal_t integer;
    };
} spk_token_literal_t;

typedef struct spk_token_s {
    SPK_token_type      type;
    char                *value;
    spk_token_literal_t literal;
    uint32_t            line;
} spk_token_t;


const char *spk_token_type_str (const SPK_token_type type);
char *spk_token_literal_to_string (const spk_token_literal_t *literal);
void spk_print_token (const spk_token_t *token);

