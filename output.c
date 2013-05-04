#include <stdio.h>
#include <stdarg.h>
#include "output.h"

void
emit_output(const char *fmt, ...)
{
    va_list args;
    fprintf(stdout, "code emit:   ");
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
    va_end(args);
}
