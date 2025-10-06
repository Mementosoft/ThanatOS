#include <string.h>  // For strcmp, strncmp
#include "kernel.h"  // For print, diskpart_select
#include "diskpart.h"  // Assurez-vous d'inclure l'en-tête de diskpart
#include "print.h"  // Assurez-vous d'inclure l'en-tête de print

int simple_atoi(const char* s);

void handle_mf_command(const char* input) {
    const char* subcmd = input + 3; // skip "mf "
    while (*subcmd == ' ') subcmd++;

    if (strncmp(subcmd, "format", 6) == 0) {
        int idx = subcmd[6] == ' ' ? simple_atoi(subcmd + 7) : -1;
        if (idx >= 0)
            diskpart_select(idx, "FAT32");
        else
            print("[mf] Missing or invalid format index.\n");
    } else if (strncmp(subcmd, "echo ", 5) == 0) {
        print(subcmd + 5);
        print("\n");
    } else {
        print("[mf] Unknown subcommand.\n");
    }
}

int simple_atoi(const char* s) {
    int n = 0;
    while (*s >= '0' && *s <= '9') {
        n = n * 10 + (*s - '0');
        s++;
    }
    return n;
}
