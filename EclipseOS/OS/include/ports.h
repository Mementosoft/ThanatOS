#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>

// Définition en inline des fonctions inb et outb
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "d"(port));
    return result;
}

static inline void outb(uint16_t port, uint8_t data) {
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "d"(port));
}

// Prototypes for 32-bit port I/O
void outportl(uint16_t port, uint32_t value);
uint32_t inportl(uint16_t port);

#endif
