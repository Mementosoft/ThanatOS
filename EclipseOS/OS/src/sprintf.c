#include "sprintf.h"
#include "tinyprintf.h"
#include <stdarg.h>

// Wrapper for tinyprintf's tfp_sprintf to provide my_sprintf
int my_sprintf(char *buffer, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = tfp_vsprintf(buffer, format, args);
    va_end(args);
    return ret;
}
