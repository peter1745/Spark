#pragma once

#include <stdint.h>

typedef enum {
    SPK_LITERAL_NUMERIC,
    SPK_LITERAL_STRING
} spk_literal_type_e;

typedef enum {
    SPK_INTEGER64,
    SPK_DOUBLE,
} spk_numeric_type_e;

typedef struct spk_numeric_literal_s {
    spk_numeric_type_e type;
    union {
        int64_t int64_val;
        double  double_val;
    };
} spk_numeric_literal_t;

typedef struct spk_string_literal_s {
    char *value;
} spk_string_literal_t;

typedef struct spk_literal_s {
    spk_literal_type_e type;
    union {
        spk_numeric_literal_t   numeric;
        spk_string_literal_t    string;
    };
} spk_literal_t;

