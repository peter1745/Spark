#pragma once

#define SPK_COUNTOF(x) (size_t)(sizeof (x) / sizeof (*x))

static const char *spk_reserved_keywords[] = {
    "var",
    "mut",
    "if",
    "else",
    "for",
    "while",
    "continue",
    "break",
    "fn",
    "return",
    "print",
};

