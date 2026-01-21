#include "token.h"

#include <stdio.h>

void
spk_print_token (const spk_token_t token)
{
#define SPK_TOKEN_TYPE(name, ...) \
    case name: \
        type = #name; \
        break;
    
    const char* type = "Unknown";
    switch (token.type) {
        SPK_TOKEN_ENUM_ITER()
        default:
            break;
    }
#undef SPK_TOKEN_TYPE

    printf ("Token(%s, %d, '%s')\n", type, token.line, token.value);
}

