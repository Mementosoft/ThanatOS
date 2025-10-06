#include "pci.h"  // Assurez-vous que pci.h déclare les fonctions comme pci_read_config, print_pci_device_info, etc.
#include "print.h"  // Si print est défini dans print.h
#include "tinyprintf.h" // Use tinyprintf for sprintf
#define sprintf tfp_sprintf

// Déclaration explicite des fonctions externes
uint32_t pci_read_config(uint16_t bus, uint16_t device, uint16_t func, uint16_t offset);
void print_pci_device_info(uint16_t bus, uint16_t device, uint16_t func, uint32_t data);
void outportl(uint16_t port, uint32_t value);
uint32_t inportl(uint16_t port);

// Fonction pour scanner les périphériques PCI
void pci_scan() {
    uint16_t bus, device, func;
    uint32_t data;
    
    // Exemple de balayage PCI (implémentation hypothétique)
    for (bus = 0; bus < 256; bus++) {
        for (device = 0; device < 32; device++) {
            for (func = 0; func < 8; func++) {
                data = pci_read_config(bus, device, func, 0);
                if (data != 0xFFFFFFFF) {  // Si un périphérique est trouvé
                    print_pci_device_info(bus, device, func, data);
                }
            }
        }
    }
}

// Fonction pour lire la configuration d'un périphérique PCI
uint32_t pci_read_config(uint16_t bus, uint16_t device, uint16_t func, uint16_t offset) {
    uint32_t address = (1 << 31) | (bus << 16) | (device << 11) | (func << 8) | (offset & 0xFC);
    outportl(0xCF8, address);
    return inportl(0xCFC);
}

// Fonction pour afficher les informations d'un périphérique PCI
void print_pci_device_info(uint16_t bus, uint16_t device, uint16_t func, uint32_t data) {
    char message[128];
    
    // Utiliser sprintf pour formater les chaînes avant de les afficher
    sprintf(message, "Périphérique PCI détecté: Bus %d, Device %d, Func %d\n", bus, device, func);
    print(message);

    sprintf(message, "ID du vendeur: 0x%X, ID du périphérique: 0x%X\n", (data >> 16) & 0xFFFF, data & 0xFFFF);
    print(message);
}
