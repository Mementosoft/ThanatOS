// input.h
#ifndef INPUT_H
#define INPUT_H

#include "config.h"  // Inclure config.h pour INPUT_BUFFER_SIZE
#include <stdint.h>   // Inclure stdint.h pour uint8_t et uint32_t
#define INPUT_BUFFER_SIZE 256  // Taille du tampon d'entrée

// Définir les types de disposition de clavier
#define LAYOUT_QWERTY  0
#define LAYOUT_AZERTY  1

extern int current_layout;  // Disposition du clavier (QWERTY ou AZERTY)
extern uint32_t input_pos;  // Déclaration correcte du type de input_pos
extern char input_buffer[INPUT_BUFFER_SIZE];  // Tampon d'entrée

void handle_input(char c);
void set_keyboard_layout(int layout);
void keyboard_handle_scancode(uint8_t scancode, void (*handler)(char));
char scancode_to_char(uint8_t scancode);  // Déclaration de la fonction

#endif // INPUT_H
