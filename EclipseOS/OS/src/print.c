#include "print.h"
#include "kernel.h"
#include <stdarg.h>
#include <stdint.h>

// Minimal VGA text mode putchar
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
static int vga_row = 0;
static int vga_col = 0;
static volatile unsigned short* vga_buffer = (unsigned short*)0xB8000;

void putchar(char c) {
    unsigned char color = 0x07; // Light grey on black
    if (c == '\n') {
        vga_row++;
        vga_col = 0;
    } else if (c == '\b') {
        if (vga_col > 0) {
            vga_col--;
            vga_buffer[vga_row * VGA_WIDTH + vga_col] = ((unsigned short)color << 8) | ' ';
        }
    } else {
        vga_buffer[vga_row * VGA_WIDTH + vga_col] = ((unsigned short)color << 8) | c;
        vga_col++;
        if (vga_col >= VGA_WIDTH) {
            vga_col = 0;
            vga_row++;
        }
    }

    if (vga_row >= VGA_HEIGHT) {
        // Scroll up
        for (int y = 1; y < VGA_HEIGHT; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
            }
        }
        // Clear the last line
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ((unsigned short)color << 8) | ' ';
        }
        vga_row = VGA_HEIGHT - 1;
    }
}

// Fonction pour afficher une chaîne de caractères
void print(const char* str) {
    while (*str) {
        putchar(*str);  // Afficher chaque caractère de la chaîne
        str++;
    }
}

// Fonction pour afficher un nombre hexadécimal
void print_hex(uint32_t num) {
    const char* hex_chars = "0123456789ABCDEF";
    char buffer[9];  // Un nombre hexadécimal de 32 bits peut avoir jusqu'à 8 chiffres + '\0'
    int i = 7;
    buffer[8] = '\0';  // Ajouter le caractère de fin de chaîne
    while (i >= 0) {
        buffer[i] = hex_chars[num & 0xF];  // Prendre les 4 derniers bits et les convertir en hex
        num >>= 4;  // Décaler de 4 bits pour examiner les suivants
        i--;
    }
    print(buffer);  // Afficher le nombre hexadécimal
}

// Fonction pour afficher un nombre décimal (entier)
void print_dec(int num) {
    if (num < 0) {
        putchar('-');  // Afficher le signe négatif pour les nombres négatifs
        num = -num;  // Rendre le nombre positif pour l'affichage
    }
    if (num == 0) {
        putchar('0');
        return;
    }
    char buffer[10];
    int i = 0;
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);  // Prendre le dernier chiffre
        num /= 10;  // Diviser le nombre par 10 pour passer au chiffre suivant
    }
    // Afficher les chiffres dans l'ordre inverse
    for (int j = i - 1; j >= 0; j--) {
        putchar(buffer[j]);
    }
}

// Fonction pour afficher un nombre en base 10
void my_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;  // Passer le '%'

            // Vérifier le type de format
            if (*format == 'd') {
                int val = va_arg(args, int);
                print_dec(val);  // Afficher un entier en base 10
            } else if (*format == 'x') {
                uint32_t val = va_arg(args, uint32_t);
                print_hex(val);  // Afficher un nombre en hexadécimal
            } else if (*format == 's') {
                char* str = va_arg(args, char*);
                print(str);  // Afficher une chaîne de caractères
            } else if (*format == 'c') {
                char c = (char) va_arg(args, int);
                putchar(c);  // Afficher un caractère
            }
        } else {
            putchar(*format);  // Afficher un caractère non spécial
        }
        format++;
    }

    va_end(args);
}
