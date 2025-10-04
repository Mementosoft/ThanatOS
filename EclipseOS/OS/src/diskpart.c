#include "diskpart.h"
#include "sprintf.h"  // Inclure l'en-tête pour my_sprintf, ou tout autre fonction de formatage
#include "print.h"    // Inclure l'en-tête de print si nécessaire

void diskpart_format(const char* filesystem) {
    // Formater le disque avec un système de fichiers spécifié
    char message[128];
    my_sprintf(message, "Formater le disque en %s...\n", filesystem);  // Utilisation de my_sprintf
    print(message);  // Utilisation de print pour afficher le message
}

void diskpart_select(int disk_id, const char* format) {
    // Sélectionner un disque avec un ID spécifié
    char message[128];
    my_sprintf(message, "Disque %d sélectionné.\n", disk_id);  // Utilisation de my_sprintf
    print(message);  // Afficher le message de sélection du disque
}
