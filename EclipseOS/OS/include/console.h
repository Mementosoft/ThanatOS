#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#define PAGE_UP 0x80
#define PAGE_DOWN 0x81
#include <stdbool.h>
#include "kernel.h"  // Inclure kernel.h pour utiliser KeyboardLayout et current_layout
#include "config.h" 
#define INPUT_BUFFER_SIZE 256  // Définir directement ici

// Déclaration des fonctions

void print_prompt(void);                        // Affiche l'invite de commande
void clear_input(void);                         // Efface le tampon d'entrée
void handle_command(const char* cmd);           // Gère les commandes de l'utilisateur
void handle_input(char c);                      // Gère l'entrée du clavier
void launch_console(void);                      // Lance la console interactive
void display_device_page(void);                 // Affiche la page des périphériques de stockage
extern char input_buffer[INPUT_BUFFER_SIZE];    // Tampon d'entrée
extern uint32_t input_pos;                      // Position actuelle dans le tampon d'entrée
void diskpart_format(const char* filesystem);   // Formate le disque avec un système de fichiers
void diskpart_select(int disk_id, const char* format);  // Utilise int disk_id et const char* format

// Déclaration de la variable de contrôle de la boucle de la console
extern bool running;  // Contrôle de l'exécution de la boucle de la console

#endif // CONSOLE_H
