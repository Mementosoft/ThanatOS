#include <stdint.h>
// video.h
#ifndef VIDEO_H
#define VIDEO_H

// Définir des couleurs pour les impressions
#define COLOR_YELLOW 1
#define COLOR_CYAN   2
#define COLOR_LIGHT_GREY 7  // Exemple de valeur pour une couleur
#define COLOR_GREEN  10     // Green color
#define COLOR_RED    12     // Red color


void scroll_up(void);
void scroll_down(void);
void clear_screen(void);
void print_color(const char* str, uint8_t color);  // Changez "int" en "uint8_t"


#endif // VIDEO_H
