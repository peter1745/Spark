#include "token.h"

#include <stdio.h>

void
Token_Dump (const spk_token_t *token)
{
    printf ("%s", Token_Type_ToString (token->type));
    switch (token->type) {
        case SPK_TOKEN_number: {
            switch (token->number.value.type) {
                case SPK_INTEGER64:
                    printf ("(%ld)", token->number.value.int64_val);
                    break;
                case SPK_DOUBLE:
                    printf ("(%lf)", token->number.value.double_val);
                    break;
            }
            break;
        }
        default:
            break;
    }

    printf ("\n");
}

