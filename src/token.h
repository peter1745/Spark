#pragma once

#define __SPK_TOKEN_DEFS
#include "token_list.h"

#define SPK_TOKEN(name, ...) SPK_TOKEN_##name,
typedef enum {
    #include "token_list.h"
} spk_token_type_e;

static inline const char *
Token_Type_ToString (spk_token_type_e type)
{
#define SPK_TOKEN(name, ...) case SPK_TOKEN_##name: return "SPK_TOKEN_"#name;
    switch (type) {
        #include "token_list.h"
    }
#undef SPK_TOKEN

    return "Unknown";
}

typedef struct spk_token_s {
    spk_token_type_e type;

#define SPK_TOKEN(name, ...) spk_##name##_token_t name;
    union {
        #include "token_list.h"
    };
} spk_token_t;

void Token_Dump (const spk_token_t *token);

