#include "scanner.h"
#include "utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct spk_file_s {
    char  *data;
    size_t size;
} spk_file_t;

typedef struct spk_option_s {
    const char *name;
    const char *long_name;
    const char *desc;
    bool *value;
} spk_option_t;

static const char *file_arg = nullptr;
static bool dump_tokens = false;

void
parse_args (int32_t argc, const char **argv)
{
#define SPK_OPTION(...) (spk_option_t) { __VA_ARGS__ }
    spk_option_t options[] = {
        SPK_OPTION ("dt", "dump-tokens", "Dumps scanned tokens to console", &dump_tokens)
    };

    if (argc == 1 || SPK_StringEq(argv[1], "-h") || SPK_StringEq(argv[1], "--help")) {
        printf ("Usage: SparkVM [options...] <file>\n\n");

        printf ("Options:\n");
        for (size_t i = 0; i < SPK_CountOf (options); ++i) {
            auto opt = &options[i];
            printf ("-%s, --%s - %s\n", opt->name, opt->long_name, opt->desc);
        }

        exit (EXIT_SUCCESS);
        return;
    }

    file_arg = argv[argc - 1];

    for (int32_t i = 1; i < argc - 1; ++i) {
        const char *arg = argv[i];

        if (arg[0] != '-') {
            printf ("Invalid argument '%s'\n", arg);
            continue;
        }

        for (size_t j = 0; j < SPK_CountOf (options); ++j) {
            auto opt = &options[j];
            bool matches_short = SPK_StringEq (arg + 1, opt->name);
            bool matches_long = arg[1] == '-' && SPK_StringEq (arg + 2, opt->long_name);

            if (!matches_short && !matches_long) {
                printf ("Unknown argument '%s'\n", arg);
                continue;
            }

            *opt->value = true;
            break;
        }
    }
}

static spk_file_t
read_file (const char *fpath)
{
    spk_file_t result = { nullptr, 0 };

    auto file = fopen (fpath, "r");
    if (!file) {
        printf ("Failed to read file %s. No such file exists.\n", fpath);
        return result;
    }

    fseek (file, 0, SEEK_END);
    auto len = ftell (file) / sizeof (char);
    fseek (file, 0, SEEK_SET);

    char *buf = calloc (len + 1, sizeof (char));
    fread (buf, sizeof (char), len, file);
    
    if (ferror (file) != 0) {
        printf ("Failed to read file '%s', an error occurred.\n", fpath);
        fclose (file);
        return result;
    }

    fclose (file);

    result.data = buf;
    result.size = len * sizeof (char);
    assert (result.data[result.size] == '\0');
    return result;
}

int
main (int argc, const char **argv)
{
    parse_args (argc, argv);

    auto file = read_file (file_arg);
    if (!file.data) {
        return EXIT_FAILURE;
    }
    auto scanner = Scanner_New (file_arg, file.data);
    free (file.data);
    
    Scanner_GenerateTokens (scanner);

    if (dump_tokens) {
        Scanner_DumpTokens (scanner);
    }

    Scanner_Free (scanner);

    return EXIT_SUCCESS;
}

