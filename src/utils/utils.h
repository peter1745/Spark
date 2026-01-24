#pragma once

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>

static inline bool
SPK_IsAlpha (char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
}

static inline bool
SPK_IsDigit (char c)
{
    return isdigit ((unsigned char)c);
}

static inline bool
SPK_IsAlphaNumeric (char c)
{
    return SPK_IsAlpha (c) || SPK_IsDigit (c);
}

static inline int64_t
SPK_StringToInt64 (char *beg, char *end)
{
    int64_t result = 0;
    for (size_t i = 0; i < (size_t)(end - beg); ++i) {
        result = result * 10 + (beg[i] - '0');
    }
    return result;
}

