// Needed for uint16_t and uint32_t
#include <stdint.h>
// 32-bit port output
void outportl(uint16_t port, uint32_t value) {
	__asm__ __volatile__("outl %0, %1" : : "a"(value), "d"(port));
}

// 32-bit port input
uint32_t inportl(uint16_t port) {
	uint32_t result;
	__asm__ __volatile__("inl %1, %0" : "=a"(result) : "d"(port));
	return result;
}
#include <stdint.h>
#include "ports.h"