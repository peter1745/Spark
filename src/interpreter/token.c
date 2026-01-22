#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
spk_token_literal_to_string (const spk_token_literal_t *literal)
{
    static char integer_buf[50] = { 0 };
    char *str = nullptr;
    switch (literal->type) {
        case SPK_TOKEN_LITERAL_EMPTY:
            break;
        case SPK_TOKEN_LITERAL_STRING:
            str = literal->string.value;
            break;
        case SPK_TOKEN_LITERAL_INTEGER:
            snprintf (integer_buf, sizeof (integer_buf),
                      "%d", literal->integer.value);
            str = integer_buf;
            break;
    }
    return str ? strdup (str) : nullptr;
}

const char *
spk_token_type_str (const SPK_token_type type)
{
#define SPK_TOKEN_TYPE(name, ...) \
    case name: \
        str = #name; \
        break;
    
    const char* str = "Unknown";
    switch (type) {
        SPK_TOKEN_ENUM_ITER()
        default:
            break;
    }
#undef SPK_TOKEN_TYPE

    return str;
}

void
spk_print_token (const spk_token_t *token) 
{
    auto type = spk_token_type_str (token->type);
    char *literal = spk_token_literal_to_string (&token->literal);
    printf ("Token(%s, %d, '%s', %s)\n", type, token->line, token->value, literal);
    free (literal);
}

