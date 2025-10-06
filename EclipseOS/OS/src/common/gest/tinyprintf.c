
#include "tinyprintf.h"
#include <stdarg.h>
#include <stddef.h>

// --- Minimal putc for kernel (replace with your own if needed) ---
static void null_putc(void* p, char c) { (void)p; (void)c; }
static putcf kernel_putc = null_putc;
static void* kernel_putp = NULL;

void init_printf(void* putp, putcf putf) {
    kernel_putp = putp;
    kernel_putc = putf;
}

void tfp_format(void* putp, putcf putf, const char* fmt, va_list va);

// --- printf ---
void tfp_printf(char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    tfp_format(kernel_putp, kernel_putc, fmt, va);
    va_end(va);
}

// --- sprintf ---
static void _putcf_buffer(void* p, char c) {
    *(*(char**)p)++ = c;
}

int tfp_vsprintf(char* str, const char* fmt, va_list ap) {
    char* buf = str;
    tfp_format(&buf, _putcf_buffer, fmt, ap);
    *buf = '\0';
    return (int)(buf - str);
}

int tfp_sprintf(char* str, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = tfp_vsprintf(str, fmt, ap);
    va_end(ap);
    return ret;
}

// --- Minimal tfp_format implementation (stub, not full) ---
void tfp_format(void* putp, putcf putf, const char* fmt, va_list va) {
    // This is a stub. For full functionality, use the original tinyprintf.c.
    // Here, just print literal strings for demonstration.
    while (*fmt) {
        if (*fmt == '%' && *(fmt+1) == 's') {
            const char* s = va_arg(va, const char*);
            while (*s) putf(putp, *s++);
            fmt += 2;
        } else {
            putf(putp, *fmt++);
        }
    }
}
