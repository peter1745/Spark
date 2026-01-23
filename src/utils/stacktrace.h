#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if defined(__GLIBC__)

#include <execinfo.h>

static constexpr size_t SPK_MAX_STACKTRACE_ENTRIES = 12;

inline static char *
__spk_stacktrace_entry_addr (char *entry)
{
    auto addr = strtok (entry, "(");
    addr = strtok (nullptr, ")");
    return addr;
}

inline static void
spk_dump_stacktrace ()
{
    // NOTE: This implementation isn't strictly *great* but
    //       it works for now.

    printf ("Stacktrace:\n");
    static void *addrs[SPK_MAX_STACKTRACE_ENTRIES] = { 0 };
    auto num_entries = backtrace (addrs, SPK_MAX_STACKTRACE_ENTRIES);
    auto symbols = backtrace_symbols (addrs, num_entries);
    for (int32_t i = 1; i < num_entries; ++i) {
        auto addr = __spk_stacktrace_entry_addr (symbols[i]);
        static char buf[256] = { 0 };
        memset (buf, 0, sizeof (buf));
        snprintf (buf, sizeof (buf), "addr2line -f -C -i -e bld/spk-interp %s", addr);
        auto fd = popen (buf, "r");
        memset (buf, 0, sizeof (buf));

        // Store function name in first half of buffer
        fgets (buf, 128, fd);
        *strchr (buf, '\n') = 0;

        // Store file path in second half of buffer
        fgets (buf + 128, 128, fd);
        *strchr (buf + 128, '\n') = 0;

        printf ("\t%d> %s: %s\n", i - 1, buf, buf + 128);
        pclose (fd);
    }
    free (symbols);
}

#else

inline static void
spk_dump_stacktrace ()
{
}

#endif

