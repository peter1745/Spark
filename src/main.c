#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "interpreter/token.h"
#include "interpreter/lexer.h"
#include "interpreter/parser.h"

/*
 - Lexing / Scanning:
    Takes in source code and tokenizes it into a more
    computationally useful format.
 - Parsing:
    Takes in a list of tokens and produces an Abstract Syntax Tree (AST)
    which represents the "grammar" of the language.
    It gives structure to the tokens.
 - Static Analysis:
    - Binding / Resolution: Figure out *where* a given identifier is defined
      and link the two together. Assigns scope to an identifier.
      Performs type checking. (Statically typed)

 - Expression: Produces a value
 - Statement:  Produces an "effect". E.g "print" produces text on the screen

 - Argument: Value you pass to a function *call*
 - Parameter: Variable that holds the passed argument value.
              Is part of a function *declaration*
*/

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
spk_read_file (const char *fpath)
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

static int32_t
spk_execute_file (const char *fpath)
{
    auto file = spk_read_file (fpath);
    if (!file.data) {
        printf ("Failed reading spk file, exiting...\n");
        return EXIT_FAILURE;
    }

    printf ("Successfully loaded file '%s'\n", fpath);

    auto tokens = spk_tokenize_source (file.data, file.size);
    if (!tokens) {
        printf ("Lexer exited with errors.\n");
        return EXIT_FAILURE;
    }
    /*for (size_t i = 0; i < tokens->count; ++i) {
        spk_print_token (darray_elem (tokens, i));
    }*/

    spk_parser_recursive_descent (tokens);
    darray_free (tokens);

    free (file.data);
    return EXIT_SUCCESS;
}

int
main (int argc, char **argv)
{
    if (argc <= 1) {
        print_help ();
        return EXIT_SUCCESS;
    }

    return spk_execute_file (argv[1]);
}

