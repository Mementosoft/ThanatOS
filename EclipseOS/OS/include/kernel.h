#ifndef KERNEL_H
#define KERNEL_H
#include "config.h" 
#include <stdbool.h>  // Inclure bool
#include <stdint.h> 

// Déclaration externe de `running` et `input_pos`
extern bool running;  // Variable pour savoir si le noyau doit continuer à tourner
extern int system_action;  // 0: normal, 1: shutdown, 2: reboot
extern uint32_t input_pos;
extern char input_buffer[INPUT_BUFFER_SIZE];  // Tampon d'entrée
char get_key_input(void); 
void keyboard_init(void);
void kernel_main(void);             // Fonction principale du noyau
void print_prompt(void);            // Fonction pour afficher l'invite de commande
void handle_input(char c);          // Fonction pour traiter l'entrée clavier
void keyboard_init(void);           // Fonction pour initialiser l'entrée clavier

#endif // KERNEL_H
