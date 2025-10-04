// keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H

#define LAYOUT_QWERTY  0
#define LAYOUT_AZERTY  1

extern int current_layout;  // Déclaration externe de current_layout

void set_keyboard_layout(int layout);

#endif // KEYBOARD_H
