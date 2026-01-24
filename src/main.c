#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "scanner.h"

static void
print_help ()
{
    printf ("Usage: spk-interp <file>\n");
}

typedef struct spk_file_s {
    char  *data;
    size_t size;
} spk_file_t;

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
main (int argc, char **argv)
{
    if (argc <= 1) {
        print_help ();
        return EXIT_SUCCESS;
    }

    auto file = read_file (argv[1]);
    if (!file.data) {
        return EXIT_FAILURE;
    }
    auto scanner = Scanner_New (argv[1], file.data);
    free (file.data);
    
    Scanner_GenerateTokens (scanner);
    Scanner_DumpTokens (scanner);

    Scanner_Free (scanner);

    return EXIT_SUCCESS;
}

