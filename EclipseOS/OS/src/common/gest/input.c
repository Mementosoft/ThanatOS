#include "console.h"
// input.c
#include "input.h"
#include "keyboard.h"  // Pour accéder à la gestion du clavier (disposition)
#include "print.h"     // Pour l'affichage des messages
#include "ports.h"     // Pour les entrées/sorties au niveau matériel
#include <string.h>    // Pour les fonctions de manipulation de chaînes
#include <stdint.h>   // Inclure stdint.h pour uint8_t et uint32_t
// Déclaration des variables externes
int current_layout = LAYOUT_QWERTY;  // Disposition de clavier par défaut (QWERTY)

uint32_t input_pos = 0;  // Définir correctement input_pos comme uint32_t
char input_buffer[INPUT_BUFFER_SIZE];  // Tampon pour la saisie des commandes

// Modifier key states
static uint8_t modifier_state = 0;
#define MODIFIER_LCTRL  0x01
#define MODIFIER_RCTRL  0x02
#define MODIFIER_LSHIFT 0x04
#define MODIFIER_RSHIFT 0x08
#define MODIFIER_LALT   0x10
#define MODIFIER_RALT   0x20

#define MODIFIER_CTRL  (MODIFIER_LCTRL | MODIFIER_RCTRL)
#define MODIFIER_SHIFT (MODIFIER_LSHIFT | MODIFIER_RSHIFT)
#define MODIFIER_ALT   (MODIFIER_LALT | MODIFIER_RALT)

void keyboard_handle_scancode(uint8_t scancode, void (*handler)(char)) {
    // Handle modifier key press/release
    uint8_t base_scancode = scancode & 0x7F; // Remove release bit
    uint8_t is_release = scancode & 0x80;    // Check if it's a release event

    // Update modifier states
    switch (base_scancode) {
        case 0x1D: // Left Ctrl
            if (is_release) modifier_state &= ~MODIFIER_LCTRL;
            else modifier_state |= MODIFIER_LCTRL;
            return; // Don't send character for modifier keys
        case 0x2A: // Left Shift
            if (is_release) modifier_state &= ~MODIFIER_LSHIFT;
            else modifier_state |= MODIFIER_LSHIFT;
            return;
        case 0x36: // Right Shift
            if (is_release) modifier_state &= ~MODIFIER_RSHIFT;
            else modifier_state |= MODIFIER_RSHIFT;
            return;
        case 0x38: // Left Alt
            if (is_release) modifier_state &= ~MODIFIER_LALT;
            else modifier_state |= MODIFIER_LALT;
            return;
    }

    // Only process key press events (not releases) for regular keys
    if (!is_release) {
        char key = scancode_to_char(base_scancode);
        if (key != '\0') {
            handler(key);
        }
    }
}

void set_keyboard_layout(int layout) {
    current_layout = layout;  // Modifier la disposition du clavier
}

// Simple polling version: waits for a keypress from the keyboard controller (port 0x60)
#define KEYBOARD_DATA_PORT 0x60
char get_key_input(void) {
    unsigned char status;
    char key = '\0';
    do {
        status = inb(0x64);
        if (status & 0x01) {
            uint8_t scancode = inb(KEYBOARD_DATA_PORT);
            key = scancode_to_char(scancode);
        }
    } while (key == '\0');
    return key;
}

// Convertir un scancode en un caractère basé sur la disposition
// QWERTY US scancode to ASCII table (set 1, no shift)
static const char scancode_ascii[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', /* 0x0E: Backspace */
    '\t', /* 0x0F: Tab */
    'q','w','e','r','t','y','u','i','o','p','[',']','\n', /* 0x1C: Enter */
    0, /* Ctrl */
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, /* LShift */
    '\\','z','x','c','v','b','n','m',',','.','/', 0, /* RShift */
    '*', 0, /* Alt */
    ' ', /* Space */
    0, /* CapsLock */ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

char scancode_to_char(uint8_t scancode) {
    if (scancode == 0x49) return PAGE_UP; // Page Up
    if (scancode == 0x51) return PAGE_DOWN; // Page Down

    if (scancode > 0 && scancode < 128) {
        char base_char = scancode_ascii[scancode];

        // Handle Ctrl combinations
        if (modifier_state & MODIFIER_CTRL) {
            if (base_char >= 'a' && base_char <= 'z') {
                return base_char - 'a' + 1; // Ctrl+A = 1, Ctrl+B = 2, etc.
            }
            if (base_char >= 'A' && base_char <= 'Z') {
                return base_char - 'A' + 1; // Same for uppercase
            }
            // Special Ctrl combinations
            switch (base_char) {
                case '[': return 0x1B; // Ctrl+[ = ESC
                case '\\': return 0x1C; // Ctrl+\
                case ']': return 0x1D; // Ctrl+]
                case '^': return 0x1E; // Ctrl+^
                case '_': return 0x1F; // Ctrl+_
            }
        }

        // Handle Shift combinations (uppercase letters)
        if (modifier_state & MODIFIER_SHIFT) {
            if (base_char >= 'a' && base_char <= 'z') {
                return base_char - 'a' + 'A'; // Convert to uppercase
            }
            // Handle shifted symbols
            switch (scancode) {
                case 0x02: return '!'; // 1 -> !
                case 0x03: return '@'; // 2 -> @
                case 0x04: return '#'; // 3 -> #
                case 0x05: return '$'; // 4 -> $
                case 0x06: return '%'; // 5 -> %
                case 0x07: return '^'; // 6 -> ^
                case 0x08: return '&'; // 7 -> &
                case 0x09: return '*'; // 8 -> *
                case 0x0A: return '('; // 9 -> (
                case 0x0B: return ')'; // 0 -> )
                case 0x0C: return '_'; // - -> _
                case 0x0D: return '+'; // = -> +
                case 0x1A: return '{'; // [ -> {
                case 0x1B: return '}'; // ] -> }
                case 0x27: return '|'; // \ -> |
                case 0x28: return ':'; // ; -> :
                case 0x29: return '"'; // ' -> "
                case 0x2B: return '<'; // , -> <
                case 0x2C: return '>'; // . -> >
                case 0x2D: return '?'; // / -> ?
                case 0x33: return '~'; // ` -> ~
            }
        }

        // Handle Alt combinations (just return the base character for now)
        if (modifier_state & MODIFIER_ALT) {
            // Alt combinations could be handled here if needed
            return base_char;
        }

        return base_char;
    }
    return '\0';
}
