#ifndef PCI_H
#define PCI_H

#include <stdint.h>  // Pour les types uint8_t, uint16_t, uint32_t

// Déclaration de la fonction pour lire les registres de configuration PCI
uint32_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

// Fonction pour scanner les périphériques PCI
void pci_scan(void);

// Déclaration des fonctions d'entrée/sortie
void outb(uint16_t port, uint8_t val);
void outw(uint16_t port, uint16_t val);
void outl(uint16_t port, uint32_t val);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t inl(uint16_t port);

#endif // PCI_H
