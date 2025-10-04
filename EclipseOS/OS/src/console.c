#include "print.h"

#include "input.h"
#include <stdbool.h>
#include "kernel.h"

extern void print_welcome_message(void);
void launch_console(void) {
	print_welcome_message();
	while (running) {
		char c = get_key_input();
		handle_input(c);
	}
}
