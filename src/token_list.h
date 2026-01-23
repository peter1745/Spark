#ifdef __SPK_TOKEN_DEFS
    #define SPK_TOKEN(name, ...) typedef struct spk_##name##_token_s { \
        __VA_ARGS__ \
    } spk_##name##_token_t;
#endif

#ifndef SPK_TOKEN
    #define SPK_TOKEN(...)
#endif

#define __SPK_DATALESS_TOKEN char unused;

SPK_TOKEN(semicolon, __SPK_DATALESS_TOKEN)

#undef SPK_TOKEN
#undef __SPK_TOKEN_DEFS

