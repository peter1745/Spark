#ifdef __SPK_TOKEN_DEFS
    #include "literals.h"

    #define SPK_TOKEN(name, ...) typedef struct spk_##name##_token_s { \
        __VA_ARGS__ \
    } spk_##name##_token_t;
#endif

#ifndef SPK_TOKEN
    #define SPK_TOKEN(...)
#endif

#define __SPK_DATALESS_TOKEN char unused;

SPK_TOKEN(number, spk_numeric_literal_t value;)
SPK_TOKEN(string, spk_string_literal_t value;)
SPK_TOKEN(semicolon, __SPK_DATALESS_TOKEN)
SPK_TOKEN(add, __SPK_DATALESS_TOKEN)
SPK_TOKEN(sub, __SPK_DATALESS_TOKEN)
SPK_TOKEN(mul, __SPK_DATALESS_TOKEN)
SPK_TOKEN(div, __SPK_DATALESS_TOKEN)
SPK_TOKEN(eof, __SPK_DATALESS_TOKEN)

#undef SPK_TOKEN
#undef __SPK_TOKEN_DEFS

