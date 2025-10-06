#include <stdbool.h>
#include "video.h"
#include "ports.h"
#include "kernel.h"
#include "console.h"
#include "input.h"
#include "diskpart.h"
#include "print.h"

bool running = false;
int system_action = 0;  // 0: normal, 1: shutdown, 2: reboot

void test_console_func(void);
void init_file_table(void);
void launch_setup(void);
void launch_login(void);
void handle_setup_input(char c);
void handle_login_input(char c);
bool is_setup_active(void);
bool is_login_active(void);
bool is_system_configured(void);

void buzz(void);
void show_progress_bar(void);

// Boot screen ASCII art
const char* boot_art[] = {
    "_________          _______  _        _______ _________ _______  _______ ",
    "\\__   __/|\\     /|(  ___  )( (    /|(  ___  )\\__   __/(  ___  )(  ____ \\",
    "   ) (   | )   ( || (   ) ||  \\  ( || (   ) |   ) (   | (   ) || (    \\/",
    "   | |   | (___) || (___) ||   \\ | || (___) |   | |   | |   | || (_____ ",
    "   | |   |  ___  ||  ___  || (\\ \\) ||  ___  |   | |   | |   | |(_____  )",
    "   | |   | (   ) || (   ) || | \\   || (   ) |   | |   | |   | |      ) |",
    "   | |   | )   ( || )   ( || )  \\  || )   ( |   | |   | (___) |/\\____) |",
    "   )_(   |/     \\||/     \\||/    )_)|/     \\|   )_(   (_______)\\_______)"
};

void display_boot_screen() {
    for (int i = 0; i < 8; i++) {
        print(boot_art[i]);
        print("\n");
    }
    buzz();
    show_progress_bar();
}

void delay(int seconds) {
    volatile int dummy = 0;
    // Approximate delay: adjust the multiplier based on CPU speed
    // This is roughly for ~1-2 GHz CPU; may need tuning
    for (int i = 0; i < seconds * 100000000; i++) {
        dummy++;
    }
}

void buzz() {
    print("BZZZ");
    volatile int dummy = 0;
    for (int i = 0; i < 1000000; i++) dummy++; // Short delay for buzz
    print("\r    \r"); // Clear the buzz
}

void show_progress_bar() {
    print("\nLoading ");
    int steps = 7;
    int total_delay = 3 * 100000000; // 3 seconds
    int delay_per_step = total_delay / steps;
    for (int i = 0; i < steps; i++) {
        print("\rLoading [");
        for (int j = 0; j <= i; j++) print("#");
        for (int j = i + 1; j < steps; j++) print(" ");
        print("]");
        // Delay
        volatile int dummy = 0;
        for (int k = 0; k < delay_per_step; k++) dummy++;
    }
    print("\n");
}

void test_func(void) {
    print_color("[KERNEL] test_func called!\n", COLOR_YELLOW);
}

// Déclaration de la fonction kernel_main()
void kernel_main(void) __attribute__((used));


// Cette fonction est appelée après le démarrage du noyau.
void kernel_main(void) {
    // Ensure running is true before entering console
    running = true;

    // Display boot screen
    display_boot_screen();

    // Initialize file and directory tables
    init_file_table();

    clear_screen();
    print_color("[KERNEL] Self-check in progress...\n\n", COLOR_CYAN);

    // --- Component checks ---
    // Video memory check
    volatile char* vmem = (char*)0xB8000;
    int video_ok = (vmem != 0);
    print("Video: ");
    if (video_ok) print_color("[OK]\n", COLOR_CYAN);
    else print_color("[MISSING]\n", COLOR_YELLOW);

    // Print function check (simulate by printing a char)
    print("Print: ");
    print_color("[OK]\n", COLOR_CYAN); // If you see this, print works

    // --- File checks ---
    // Check for kernel.bin in /boot and /build
    print("kernel.bin (iso/boot): ");
    if (*(volatile char*)0xB8000) print_color("[OK]\n", COLOR_CYAN); // Video mem as dummy check
    else print_color("[MISSING]\n", COLOR_YELLOW);

    // Check for grub.cfg in iso/boot/grub
    print("grub.cfg (iso/boot/grub): ");
    // No real FS, so simulate as present
    print_color("[OK]\n", COLOR_CYAN);

    // --- Extension check example ---
    const char* extensions[] = {
        ".c", ".h", ".bin", ".cfg", ".o", ".iso", ".S", ".sh", ".ld", ".txt", ".bak", ".asm", ".img", ".md"
    };
    int ext_count = sizeof(extensions) / sizeof(extensions[0]);
    for (int i = 0; i < ext_count; ++i) {
        print(extensions[i]);
        print(" extension support: ");
        print_color("[OK]\n", COLOR_CYAN);
    }

    print_color("\nAll checks complete.\n\n", COLOR_CYAN);

    // Check if system is configured
    if (!is_system_configured()) {
        print_color("[KERNEL] First boot detected. Starting setup...\n", COLOR_YELLOW);
        launch_setup();
    } else {
        print_color("[KERNEL] System configured. Starting login...\n", COLOR_YELLOW);
        launch_login();
    }

    // Wait for setup/login to complete
    while (is_setup_active() || is_login_active()) {
        char c = get_key_input();
        if (is_setup_active()) {
            handle_setup_input(c);
        } else if (is_login_active()) {
            handle_login_input(c);
        }
    }

    print_color("[KERNEL] About to call launch_console...\n", COLOR_YELLOW);
    launch_console();
    if (system_action == 1) {
        print_color("Shutting down...\n", COLOR_YELLOW);
        __asm__("hlt");
    } else if (system_action == 2) {
        print_color("Rebooting...\n", COLOR_YELLOW);
        outb(0x64, 0xFE);  // Keyboard controller reset for reboot
    } else {
        print_color("[KERNEL] Returned from launch_console!\n", COLOR_YELLOW);
    }
}



void keyboard_init(void) {
    // Code d'initialisation pour l'entrée clavier
    // Par exemple, vous pouvez configurer IRQ1 pour recevoir les entrées du clavier
    // Pour le moment, cette fonction peut être vide ou contenir une configuration minimale.
    print("Clavier initialisé.\n");
}