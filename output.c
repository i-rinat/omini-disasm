#include <stdio.h>
#include <stdarg.h>
#include "output.h"
#include <assert.h>


FILE *output_fp = NULL;

void
emit_code(const char *fmt, ...)
{
    FILE *target_fp = output_fp ? output_fp : stdout;
    va_list args;

    if (!output_fp)
        fprintf(target_fp, "code emit:   ");
    va_start(args, fmt);
    vfprintf(target_fp, fmt, args);
    fprintf(target_fp, "\n");
    va_end(args);
}

/// @brief emit code to current output. Without new line character.
void
emit_code_nonl(const char *fmt, ...)
{
    FILE *target_fp = output_fp ? output_fp : stdout;
    va_list args;

    if (!output_fp)
        fprintf(target_fp, "code emit:   ");
    va_start(args, fmt);
    vfprintf(target_fp, fmt, args);

    va_end(args);
}

void
set_output_file(const char *fname)
{
    output_fp = fopen(fname, "w");
    assert(output_fp);
}

void
close_output_file()
{
    if (output_fp)
        fclose(output_fp);
    output_fp = NULL;
}
