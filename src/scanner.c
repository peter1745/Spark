#include "scanner.h"
#include "token.h"
#include "utils/utils.h"
#include "utils/darray.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

struct spk_scanner_s {
    const char  *file;
    uint32_t    line;
    char        *source;
    char        *curr;
    char        *end;
    char        *token_start;
    darray_t    *tokens;
    uint32_t     num_errors;
};

static void
scanner_report_error (spk_scanner_t *scanner, const char *msg, char cause)
{
    printf ("Scanner Error: %s\n", msg);
    printf ("Offending Character: %c\n", cause);
    printf ("Scanner State:\n");
    printf ("\tFile: %s\n", scanner->file);
    printf ("\tLine: %u\n", scanner->line);
    printf ("\tCurr: '%c'\n", *scanner->curr);
    printf ("\tToken Start: '%c'\n", *scanner->token_start);
    scanner->num_errors++;
}

static inline bool
scanner_had_error (spk_scanner_t *scanner)
{
    return scanner->num_errors > 0;
}

static bool
scanner_at_end (spk_scanner_t *scanner)
{
    return scanner->curr >= scanner->end;
}

static char
scanner_consume (spk_scanner_t *scanner)
{
    if (scanner_at_end (scanner)) {
        return '\0';
    }

    auto c = *scanner->curr;
    scanner->curr++;
    return c;
}

spk_token_t
generate_numeric_token (spk_scanner_t *scanner)
{
    spk_numeric_type_e type = SPK_INTEGER64;

    // Work out if we're dealing with a floating-point
    // number or an integer
    while (SPK_IsDigit(*scanner->curr)) {
        scanner_consume (scanner);
    }

    if (*scanner->curr == '.') {
        // Floating-point number
        type = SPK_DOUBLE;

        scanner_consume (scanner);

        while (SPK_IsDigit(*scanner->curr)) {
            scanner_consume (scanner);
        }

        scanner_report_error (scanner, "Unsupported numeric", '.');
    }
 
    spk_numeric_literal_t literal = {
        .type = type
    };

    switch (type) {
        case SPK_INTEGER64:
            literal.int64_val = SPK_StringToInt64 (scanner->token_start, scanner->curr);
            break;
        default:
            assert (false);
    }

    return (spk_token_t) {
        .type = SPK_TOKEN_number,
        .number = {
            .value = literal
        }
    };
}

static spk_token_t
generate_string_token (spk_scanner_t *scanner)
{
    while (*scanner->curr != '"') {
        scanner_consume (scanner);
    }

    if (scanner_at_end (scanner)) {
        scanner_report_error (scanner, "Unterminated string", '"');
        return (spk_token_t) {};
    }

    // Consume closing quote
    scanner_consume (scanner);

    size_t len = (size_t)(scanner->curr - scanner->token_start);
    spk_string_literal_t string = {
        .value = calloc (len, sizeof (char)),
        .len = len
    };

    memcpy (string.value, scanner->token_start, len * sizeof (char));

    return (spk_token_t) {
        .type = SPK_TOKEN_string,
        .string = { string }
    };
}

static bool
generate_token (spk_scanner_t *scanner, spk_token_t *token)
{
    bool generated_valid_token = false;

#define __MAKE_TOKEN(t) (spk_token_t) { .type = t }
#define __SIMPLE_TOKEN(tok, c) \
    c: \
        *token = __MAKE_TOKEN (SPK_TOKEN_##tok); \
        generated_valid_token = true; \
        break

    auto c = scanner_consume (scanner);
    switch (c) {
        case '\n':
            scanner->line++;
            break;
        case ' ':
        case '\t':
        case '\r':
            break;
        case __SIMPLE_TOKEN (semicolon, ';');
        case __SIMPLE_TOKEN (add, '+');
        case __SIMPLE_TOKEN (sub, '-');
        case __SIMPLE_TOKEN (mul, '*');
        case __SIMPLE_TOKEN (div, '/');
        case __SIMPLE_TOKEN (eof, '\0');
        case '"':
            *token = generate_string_token (scanner);
            generated_valid_token = true;
            break;
        default:
            if (SPK_IsDigit (c)) {
                *token = generate_numeric_token (scanner);
                generated_valid_token = true;
            } else {
                scanner_report_error (scanner, "Unknown token", c);
            }
            break;
    }

    return generated_valid_token;
}

spk_scanner_t *
Scanner_New (const char *fp, const char *source)
{
    spk_scanner_t *scanner = calloc (1, sizeof (spk_scanner_t));
    scanner->file = fp;
    scanner->source = strdup (source);
    scanner->curr = scanner->source;
    scanner->end = scanner->source + strlen (scanner->source);
    scanner->token_start = scanner->curr;
    scanner->tokens = darray_empty (sizeof (spk_token_t));
    scanner->line = 1;
    return scanner;
}

void
Scanner_Free (spk_scanner_t *scanner)
{
    darray_free (scanner->tokens);
    free (scanner->source);
    free (scanner);
}

spk_scanner_result_e
Scanner_GenerateTokens (spk_scanner_t *scanner)
{
    spk_scanner_result_e result = SPK_SCANNER_SUCCESS;

    while (!scanner_at_end (scanner)) {
        scanner->token_start = scanner->curr;

        spk_token_t token;
        if (generate_token (scanner, &token)) {
            darray_append (scanner->tokens, &token);
        }

        if (scanner_had_error (scanner)) {
            // TODO: Do something?
            result = SPK_SCANNER_ERROR;
        }
    }

    // Ensure last token is always EOF
    darray_append_v(scanner->tokens, (spk_token_t) {
        .type = SPK_TOKEN_eof
    });

    printf ("Scanner finished with %d errors.\n", scanner->num_errors);
    return result;
}

void
Scanner_DumpTokens (spk_scanner_t *scanner)
{
    spk_token_t *token;
    darray_iter (scanner->tokens, token) {
        size_t i = (size_t)(token - (spk_token_t *)scanner->tokens->data);
        printf ("%lu: ", i);
        Token_Dump (token);
    }
}

