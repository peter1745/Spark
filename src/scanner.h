#pragma once

typedef struct spk_scanner_s spk_scanner_t;

spk_scanner_t *Scanner_New (const char *fp, const char *source);
void Scanner_Free (spk_scanner_t *scanner);

typedef enum {
    SPK_SCANNER_SUCCESS,
    SPK_SCANNER_ERROR
} spk_scanner_result_e;

spk_scanner_result_e Scanner_GenerateTokens (spk_scanner_t *scanner);

void Scanner_DumpTokens (spk_scanner_t *scanner);

