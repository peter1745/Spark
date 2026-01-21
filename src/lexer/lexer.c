#include "lexer.h"
#include "../keywords.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint32_t curr_line = 1;
static size_t token_list_capacity = 0;

typedef struct spk_lexer_ctx_s {
    const char *source;
    
    const char *start;
    const char *current;

    spk_token_list_t tokens;
} spk_lexer_ctx_t;

static inline bool
spk_lexer_at_end (spk_lexer_ctx_t *ctx)
{
    return *(ctx->current) == '\0';
}

static char
spk_lexer_advance (spk_lexer_ctx_t *ctx)
{
    // TODO: If at end, error
    return *(ctx->current++);
}

static void
spk_token_list_grow (spk_token_list_t *list, size_t cap)
{
    if (cap < token_list_capacity) {
        return;
    }

    list->elems = reallocarray (list->elems, cap, sizeof (spk_token_t));
    memset (list->elems + token_list_capacity, 0,
            (cap - token_list_capacity) * sizeof (spk_token_t));
    token_list_capacity = cap;
}

static void
spk_lexer_report_err (uint32_t line, const char *msg)
{
    printf ("Error: %s, line %d\n", msg, line);
}

static void
spk_insert_token (spk_lexer_ctx_t *ctx, SPK_token_type type)
{
    auto list = &ctx->tokens;

    if (list->count + 1 >= token_list_capacity) {
        spk_token_list_grow(list, token_list_capacity +
                                 (token_list_capacity / 2));
    }

    char *buf = nullptr;
    if (!spk_lexer_at_end (ctx) && ctx->start && ctx->current) {
        size_t len = (size_t)(ctx->current - ctx->start) + 1;
        buf = calloc (len, sizeof (char));
        memcpy (buf, ctx->start, len - 1);
    }

    if (type == SPK_TOKEN_TYPE_IDENTIFIER) {
        // Check if buf matches any keywords
        for (size_t i = 0; i < SPK_COUNTOF (spk_reserved_keywords); ++i) {
            if (strcmp (spk_reserved_keywords[i], buf) == 0) {
                type = SPK_TOKEN_TYPE_KEYWORD;
                break;
            }
        }
    }

    list->elems[list->count] = (spk_token_t) {
        .type = type,
        .value = buf,
        .line = curr_line
    };
    list->count++;
}

static void
spk_consume_comment (spk_lexer_ctx_t *ctx)
{
    if (spk_lexer_at_end (ctx)) {
        spk_lexer_report_err (curr_line,
                              "Tried to consume comment at end of file");
        return;
    }
    
    while (*ctx->current != '\n') {
        ctx->current++;
    }
}

static void
spk_try_consume_string (spk_lexer_ctx_t *ctx)
{
    while (!spk_lexer_at_end (ctx) && *ctx->current != '"') {
        if (spk_lexer_advance (ctx) == '\n') {
            curr_line++;
        }
    }

    if (spk_lexer_at_end (ctx)) {
        spk_lexer_report_err(curr_line, "Unterminated string");
        return;
    }

    // FIXME: Strip new lines for multi-line stings

    // Insert string ensuring we don't
    // include the quotation marks
    ctx->start++;
    spk_insert_token (ctx, SPK_TOKEN_TYPE_STRING);
    ctx->current++;
}

static inline bool
spk_is_digit (char c)
{
    return c >= '0' && c <= '9';
}

static void
spk_consume_number (spk_lexer_ctx_t *ctx)
{
    while (spk_is_digit(*ctx->current)) {
        spk_lexer_advance (ctx);
    }

    if (*ctx->current == '.') {
        spk_lexer_report_err (curr_line, "Fractional numbers not supported");

        // Consume '.'
        spk_lexer_advance (ctx);

        // Consume fractional part
        while (spk_is_digit(*ctx->current)) {
            spk_lexer_advance (ctx);
        }

        return;
    }
    
    spk_insert_token (ctx, SPK_TOKEN_TYPE_INTEGER);
}

static inline bool
spk_is_valid_ident_start (char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_'; // FIXME: Should _ be a valid ident start?
}

static inline bool
spk_is_valid_ident_char (char c)
{
    return spk_is_valid_ident_start (c) || spk_is_digit (c);
}

static void
spk_consume_identifier (spk_lexer_ctx_t *ctx)
{
    while (spk_is_valid_ident_char(*ctx->current)) {
        (void)spk_lexer_advance (ctx);
    }

    spk_insert_token (ctx, SPK_TOKEN_TYPE_IDENTIFIER);
}

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

spk_token_list_t
spk_tokenize_source (const char *src, size_t len)
{
    spk_lexer_ctx_t ctx = {
        .source = src,
        .start = src,
        .current = src,
        .tokens = { nullptr, 0 }
    };
    spk_token_list_grow (&ctx.tokens, 10);

    while (!spk_lexer_at_end (&ctx)) {
        ctx.start = ctx.current;
        auto curr = spk_lexer_advance (&ctx);

        switch (curr) {
            case '#':
                spk_consume_comment (&ctx);
                continue;
            case '"':
                spk_try_consume_string (&ctx);
                break;
            case '\n':
                curr_line++;
                break;
            case ' ':
            case '\r':
            case '\t':
                break;
            default:
                if (spk_is_digit (curr)) {
                    spk_consume_number (&ctx);
                } else if (spk_is_valid_ident_start (curr)) {
                    spk_consume_identifier (&ctx);
                } else {
                    spk_insert_token (&ctx, SPK_TOKEN_TYPE_CHAR);
                }

                break;
        }
    }

    spk_insert_token (&ctx, SPK_TOKEN_TYPE_EOF);
    return ctx.tokens;
}

void
spk_free_token_list (spk_token_list_t *list)
{
    for (size_t i = 0; i < list->count; ++i) {
        free (list->elems[i].value);
    }

    free (list->elems);
}
