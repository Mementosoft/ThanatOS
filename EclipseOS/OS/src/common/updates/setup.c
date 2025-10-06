#include "print.h"
#include "input.h"
#include "video.h"
#include "console.h"
#include <string.h>
#include <stdbool.h>

// Forward declarations for file system functions
void create_dir(const char* dirname);
bool create_file(const char* filename);
void save_user_config(const char* username, const char* password, int keyboard_layout);
bool load_user_config(char* username, char* password, int* keyboard_layout);

// Configuration structure
#define MAX_USERNAME_LEN 32
#define MAX_PASSWORD_LEN 32

typedef struct {
    int keyboard_layout;  // 0: QWERTY, 1: AZERTY
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    bool configured;
} SystemConfig;

// Global config (simulated persistent storage)
SystemConfig system_config = {0, "", "", false};

// Setup mode variables
static bool setup_mode = false;
static int setup_step = 0; // 0: keyboard, 1: username, 2: password, 3: confirm password
static char temp_username[MAX_USERNAME_LEN];
static char temp_password[MAX_PASSWORD_LEN];
static char temp_confirm[MAX_PASSWORD_LEN];
static int temp_layout = 0;

// Login mode variables
static bool login_mode = false;
static int login_step = 0; // 0: username, 1: password
static char login_username[MAX_USERNAME_LEN];
static char login_password[MAX_PASSWORD_LEN];

void save_config(void) {
    save_user_config(system_config.username, system_config.password, system_config.keyboard_layout);
    print_color("Configuration saved to user/accounts.txt!\n", COLOR_GREEN);
}

bool load_config(void) {
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    int keyboard_layout;

    if (load_user_config(username, password, &keyboard_layout)) {
        strcpy(system_config.username, username);
        strcpy(system_config.password, password);
        system_config.keyboard_layout = keyboard_layout;
        system_config.configured = true;
        return true;
    }
    return false;
}

void launch_setup(void) {
    setup_mode = true;
    setup_step = 0;
    clear_screen();

    print_color("=== ThanatOS Setup ===\n\n", COLOR_CYAN);
    print_color("Welcome to ThanatOS! Let's configure your system.\n\n", COLOR_YELLOW);

    print("Step 1: Choose keyboard layout\n");
    print("1. QWERTY (default)\n");
    print("2. AZERTY\n");
    print("\nPress 1 or 2: ");
}

void launch_login(void) {
    if (!system_config.configured) {
        launch_setup();
        return;
    }

    login_mode = true;
    login_step = 0;
    clear_screen();

    print_color("=== EclipseOS Login ===\n\n", COLOR_CYAN);
    print("Username: ");
}

void handle_setup_input(char c) {
    if (!setup_mode) return;

    switch (setup_step) {
        case 0: // Keyboard layout selection
            if (c == '1') {
                temp_layout = 0; // QWERTY
                print("1\n");
                setup_step = 1;
                print("\nStep 2: Choose username\n");
                print("Username (max 31 chars): ");
            } else if (c == '2') {
                temp_layout = 1; // AZERTY
                print("2\n");
                setup_step = 1;
                print("\nStep 2: Choose username\n");
                print("Username (max 31 chars): ");
            }
            break;

        case 1: // Username input
            if (c == '\n' || c == '\r') {
                temp_username[strlen(temp_username)] = '\0';
                if (strlen(temp_username) > 0) {
                    print("\n");
                    setup_step = 2;
                    print("Step 3: Choose password\n");
                    print("Password (max 31 chars): ");
                }
            } else if (c == '\b' && strlen(temp_username) > 0) {
                temp_username[strlen(temp_username) - 1] = '\0';
                print("\b \b");
            } else if (c >= 32 && c <= 126 && strlen(temp_username) < MAX_USERNAME_LEN - 1) {
                temp_username[strlen(temp_username)] = c;
                temp_username[strlen(temp_username) + 1] = '\0';
                print("*"); // Hide username input
            }
            break;

        case 2: // Password input
            if (c == '\n' || c == '\r') {
                temp_password[strlen(temp_password)] = '\0';
                if (strlen(temp_password) > 0) {
                    print("\n");
                    setup_step = 3;
                    print("Step 4: Confirm password\n");
                    print("Confirm password: ");
                }
            } else if (c == '\b' && strlen(temp_password) > 0) {
                temp_password[strlen(temp_password) - 1] = '\0';
                print("\b \b");
            } else if (c >= 32 && c <= 126 && strlen(temp_password) < MAX_PASSWORD_LEN - 1) {
                temp_password[strlen(temp_password)] = c;
                temp_password[strlen(temp_password) + 1] = '\0';
                print("*"); // Hide password input
            }
            break;

        case 3: // Confirm password
            if (c == '\n' || c == '\r') {
                temp_confirm[strlen(temp_confirm)] = '\0';
                if (strcmp(temp_password, temp_confirm) == 0) {
                    // Save configuration
                    system_config.keyboard_layout = temp_layout;
                    strcpy(system_config.username, temp_username);
                    strcpy(system_config.password, temp_password);
                    system_config.configured = true;
                    save_config();

                    print("\n");
                    print_color("Setup complete! Press any key to continue...\n", COLOR_GREEN);
                    setup_step = 4; // Wait for key press
                } else {
                    print("\n");
                    print_color("Passwords don't match! Try again.\n", COLOR_RED);
                    temp_confirm[0] = '\0';
                    temp_password[0] = '\0';
                    setup_step = 2;
                    print("Password (max 31 chars): ");
                }
            } else if (c == '\b' && strlen(temp_confirm) > 0) {
                temp_confirm[strlen(temp_confirm) - 1] = '\0';
                print("\b \b");
            } else if (c >= 32 && c <= 126 && strlen(temp_confirm) < MAX_PASSWORD_LEN - 1) {
                temp_confirm[strlen(temp_confirm)] = c;
                temp_confirm[strlen(temp_confirm) + 1] = '\0';
                print("*"); // Hide password input
            }
            break;

        case 4: // Setup complete, wait for key
            setup_mode = false;
            set_keyboard_layout(temp_layout == 1 ? LAYOUT_AZERTY : LAYOUT_QWERTY);
            break;
    }
}

void handle_login_input(char c) {
    if (!login_mode) return;

    switch (login_step) {
        case 0: // Username input
            if (c == '\n' || c == '\r') {
                login_username[strlen(login_username)] = '\0';
                if (strlen(login_username) > 0) {
                    print("\n");
                    login_step = 1;
                    print("Password: ");
                }
            } else if (c == '\b' && strlen(login_username) > 0) {
                login_username[strlen(login_username) - 1] = '\0';
                print("\b \b");
            } else if (c >= 32 && c <= 126 && strlen(login_username) < MAX_USERNAME_LEN - 1) {
                login_username[strlen(login_username)] = c;
                login_username[strlen(login_username) + 1] = '\0';
                char buf[2] = {c, 0};
                print(buf);
            }
            break;

        case 1: // Password input
            if (c == '\n' || c == '\r') {
                login_password[strlen(login_password)] = '\0';
                if (strcmp(login_username, system_config.username) == 0 &&
                    strcmp(login_password, system_config.password) == 0) {
                    print("\n");
                    print_color("Login successful! Welcome, ", COLOR_GREEN);
                    print_color(system_config.username, COLOR_GREEN);
                    print_color("!\n\n", COLOR_GREEN);
                    print_color("Press any key to continue...\n", COLOR_YELLOW);
                    login_step = 2; // Wait for key press
                } else {
                    print("\n");
                    print_color("Invalid username or password!\n", COLOR_RED);
                    login_username[0] = '\0';
                    login_password[0] = '\0';
                    login_step = 0;
                    print("Username: ");
                }
            } else if (c == '\b' && strlen(login_password) > 0) {
                login_password[strlen(login_password) - 1] = '\0';
                print("\b \b");
            } else if (c >= 32 && c <= 126 && strlen(login_password) < MAX_PASSWORD_LEN - 1) {
                login_password[strlen(login_password)] = c;
                login_password[strlen(login_password) + 1] = '\0';
                print("*"); // Hide password input
            }
            break;

        case 2: // Login successful, wait for key
            login_mode = false;
            set_keyboard_layout(system_config.keyboard_layout == 1 ? LAYOUT_AZERTY : LAYOUT_QWERTY);
            break;
    }
}

bool is_setup_active(void) {
    return setup_mode;
}

bool is_login_active(void) {
    return login_mode;
}

bool is_system_configured(void) {
    return system_config.configured;
}

const char* get_current_username(void) {
    return system_config.username;
}