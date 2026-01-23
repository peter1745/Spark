#pragma once

#define __SPK_TOKEN_DEFS
#include "token_list.h"

#define SPK_TOKEN(name, ...) SPK_TOKEN_##name,
typedef enum {
    #include "token_list.h"
} spk_token_type_e;

typedef struct spk_token_s {
    spk_token_type_e type;

#define SPK_TOKEN(name, ...) spk_##name##_token_t name;
    union {
        #include "token_list.h"
    };
} spk_token_t;

