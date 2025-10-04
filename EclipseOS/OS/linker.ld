ENTRY(_start)

SECTIONS
{
    . = 0x10000;  /* Adresse de début du code */

    .text : {
        *(.text)  /* Tout le code va dans la section .text */
    }

    .data : {
        *(.data)  /* Données initialisées */
    }

    .bss : {
        *(.bss)  /* Données non initialisées */
    }
    
    /* On réserve de la place pour la stack et autres sections ici si nécessaire. */
}
