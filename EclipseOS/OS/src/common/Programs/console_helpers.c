#include "input.h"
#include "print.h"
#include "console.h"
#include "video.h"
#include <string.h>
#include <stdbool.h>

// Simple implementations of string functions not available in freestanding environment
char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) {
            return (char*)s;
        }
        s++;
    }
    return (c == '\0') ? (char*)s : NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s) {
        if (*s == (char)c) {
            last = s;
        }
        s++;
    }
    return (c == '\0') ? (char*)s : (char*)last;
}

// Check if a filename corresponds to a system file
bool is_system_file(const char* filename) {
    // List of system files that cannot be modified
    const char* system_files[] = {
        "Makefile", "linker.ld", "grub.cfg", "launchqemu.sh", "arbo.txt",
        "src/kernel.c", "src/console.c", "src/console_helpers.c", "src/input.c",
        "src/print.c", "src/video.c", "src/string.c", "src/memory.c",
        "src/diskpart.c", "src/pci.c", "src/ports.c", "src/sprintf.c",
        "src/tinyprintf.c", "src/mf_stub.c", "include/kernel.h",
        "include/console.h", "include/print.h", "include/video.h",
        "include/input.h", "include/diskpart.h", "include/pci.h",
        "include/ports.h", "include/sprintf.h", "include/tinyprintf.h",
        "include/stddef.h", "include/kstring.h", "include/config.h",
        "include/keyboard.h", "boot/boot.asm", "src/multiboot_header.S",
        "build/kernel.bin", "build/boot.bin"
    };

    for (int i = 0; i < sizeof(system_files) / sizeof(system_files[0]); ++i) {
        if (strcmp(filename, system_files[i]) == 0) {
            return true;
        }
    }
    return false;
}

#define VERSION_STRING "ThanatOS (EclipseOS) v0.1\n"
#define MAX_FILES 64
#define MAX_DIRS 64
#define MAX_FILENAME_LEN 32
#define MAX_FILE_CONTENT 1024
#define MAX_USERNAME_LEN 32
#define MAX_PASSWORD_LEN 32

typedef struct {
    char name[MAX_FILENAME_LEN];
    char content[MAX_FILE_CONTENT];
    int content_length;
    bool exists;
} File;

typedef struct {
    char name[MAX_FILENAME_LEN];
    bool exists;
} Dir;

File file_table[MAX_FILES];
Dir dir_table[MAX_DIRS];
static char current_dir[64] = "/";

void init_file_table(void) {
    for (int i = 0; i < MAX_FILES; ++i) {
        file_table[i].exists = false;
        file_table[i].content[0] = '\0';
        file_table[i].content_length = 0;
    }
    for (int i = 0; i < MAX_DIRS; ++i) {
        dir_table[i].exists = false;
    }
}

bool create_file(const char* filename) {
    char full_path[MAX_FILENAME_LEN];
    if (strcmp(current_dir, "/") == 0) {
        strncpy(full_path, filename, MAX_FILENAME_LEN - 1);
    } else {
        // Create path like "dirname/filename"
        int len = strlen(current_dir);
        if (len + 1 + strlen(filename) >= MAX_FILENAME_LEN) {
            return false; // Path too long
        }
        strcpy(full_path, current_dir);
        full_path[len] = '/';
        strcpy(full_path + len + 1, filename);
    }
    full_path[MAX_FILENAME_LEN - 1] = '\0';

    for (int i = 0; i < MAX_FILES; ++i) {
        if (!file_table[i].exists) {
            strncpy(file_table[i].name, full_path, MAX_FILENAME_LEN - 1);
            file_table[i].name[MAX_FILENAME_LEN - 1] = '\0';
            file_table[i].exists = true;
            return true;
        }
    }
    return false;
}

bool create_dir(const char* dirname) {
    char full_path[MAX_FILENAME_LEN];
    if (strcmp(current_dir, "/") == 0) {
        strncpy(full_path, dirname, MAX_FILENAME_LEN - 1);
    } else {
        // Create path like "dirname/subdirname"
        int len = strlen(current_dir);
        if (len + 1 + strlen(dirname) >= MAX_FILENAME_LEN) {
            return false; // Path too long
        }
        strcpy(full_path, current_dir);
        full_path[len] = '/';
        strcpy(full_path + len + 1, dirname);
    }
    full_path[MAX_FILENAME_LEN - 1] = '\0';

    for (int i = 0; i < MAX_DIRS; ++i) {
        if (!dir_table[i].exists) {
            strncpy(dir_table[i].name, full_path, MAX_FILENAME_LEN - 1);
            dir_table[i].name[MAX_FILENAME_LEN - 1] = '\0';
            dir_table[i].exists = true;
            return true;
        }
    }
    return false;
}

bool dir_exists(const char* dirname) {
    char full_path[MAX_FILENAME_LEN];
    if (strcmp(current_dir, "/") == 0) {
        strncpy(full_path, dirname, MAX_FILENAME_LEN - 1);
    } else {
        // Create path like "current_dir/dirname"
        int len = strlen(current_dir);
        if (len + 1 + strlen(dirname) >= MAX_FILENAME_LEN) {
            return false; // Path too long
        }
        strcpy(full_path, current_dir);
        full_path[len] = '/';
        strcpy(full_path + len + 1, dirname);
    }
    full_path[MAX_FILENAME_LEN - 1] = '\0';

    for (int i = 0; i < MAX_DIRS; ++i) {
        if (dir_table[i].exists && strcmp(dir_table[i].name, full_path) == 0) {
            return true;
        }
    }
    return false;
}
bool remove_file(const char* filename) {
    for (int i = 0; i < MAX_FILES; ++i) {
        if (file_table[i].exists && strcmp(file_table[i].name, filename) == 0) {
            file_table[i].exists = false;
            return true;
        }
    }
    return false;
}

bool rename_file(const char* oldname, const char* newname) {
    // Check if newname already exists
    for (int i = 0; i < MAX_FILES; ++i) {
        if (file_table[i].exists && strcmp(file_table[i].name, newname) == 0) {
            return false; // newname already exists
        }
    }
    // Find and rename
    for (int i = 0; i < MAX_FILES; ++i) {
        if (file_table[i].exists && strcmp(file_table[i].name, oldname) == 0) {
            strncpy(file_table[i].name, newname, MAX_FILENAME_LEN - 1);
            file_table[i].name[MAX_FILENAME_LEN - 1] = '\0';
            return true;
        }
    }
    return false;
}

void list_files(void) {
    print("dir :\n");

    if (strcmp(current_dir, "/") == 0) {
        // Show system files first
        print("System Files:\n");
        print(" - Makefile\n");
        print(" - linker.ld\n");
        print(" - grub.cfg\n");
        print(" - launchqemu.sh\n");
        print(" - arbo.txt\n");
        print(" - src/kernel.c\n");
        print(" - src/console.c\n");
        print(" - src/console_helpers.c\n");
        print(" - src/input.c\n");
        print(" - src/print.c\n");
        print(" - src/video.c\n");
        print(" - src/string.c\n");
        print(" - src/memory.c\n");
        print(" - src/diskpart.c\n");
        print(" - src/pci.c\n");
        print(" - src/ports.c\n");
        print(" - src/sprintf.c\n");
        print(" - src/tinyprintf.c\n");
        print(" - src/mf_stub.c\n");
        print(" - include/kernel.h\n");
        print(" - include/console.h\n");
        print(" - include/print.h\n");
        print(" - include/video.h\n");
        print(" - include/input.h\n");
        print(" - include/diskpart.h\n");
        print(" - include/pci.h\n");
        print(" - include/ports.h\n");
        print(" - include/sprintf.h\n");
        print(" - include/tinyprintf.h\n");
        print(" - include/stddef.h\n");
        print(" - include/kstring.h\n");
        print(" - include/config.h\n");
        print(" - include/keyboard.h\n");
        print(" - boot/boot.asm\n");
        print(" - src/multiboot_header.S\n");
        print(" - build/kernel.bin\n");
        print(" - build/boot.bin\n");
        print("\n");

        // Show user-created directories and files at root level
        bool has_user_content = false;
        print("User Content:\n");
        for (int i = 0; i < MAX_DIRS; ++i) {
            if (dir_table[i].exists && strcmp(dir_table[i].name, "/") != 0) {
                if (!has_user_content) {
                    has_user_content = true;
                }
                print(" - ");
                print(dir_table[i].name);
                print("/\n");
            }
        }
        for (int i = 0; i < MAX_FILES; ++i) {
            if (file_table[i].exists && strcmp(file_table[i].name, "/") != 0) {
                if (!has_user_content) {
                    has_user_content = true;
                }
                print(" - ");
                print(file_table[i].name);
                print("\n");
            }
        }
        if (!has_user_content) {
            print(" - (no user files)\n");
        }
    } else {
        // Show current directory and its contents
        print("      ");
        print(current_dir);
        print("/\n");

        bool has_content = false;
        // Show subdirectories and files under current directory
        int current_len = strlen(current_dir);
        for (int i = 0; i < MAX_DIRS; ++i) {
            if (dir_table[i].exists &&
                strncmp(dir_table[i].name, current_dir, current_len) == 0 &&
                dir_table[i].name[current_len] == '/' &&
                strchr(dir_table[i].name + current_len + 1, '/') == NULL) {
                // This is a direct subdirectory
                if (!has_content) {
                    has_content = true;
                }
                print("                 ");
                print(strrchr(dir_table[i].name, '/') + 1);
                print("/\n");
            }
        }
        for (int i = 0; i < MAX_FILES; ++i) {
            if (file_table[i].exists &&
                strncmp(file_table[i].name, current_dir, current_len) == 0 &&
                file_table[i].name[current_len] == '/' &&
                strchr(file_table[i].name + current_len + 1, '/') == NULL) {
                // This is a direct file in current directory
                if (!has_content) {
                    has_content = true;
                }
                print("                 ");
                print(strrchr(file_table[i].name, '/') + 1);
                print("\n");
            }
        }
        if (!has_content) {
            print("                 (empty)\n");
        }
    }
    print("\n");
}

void print_welcome_message(void) {
    clear_screen();
    const char* lines[] = {
        "        ,----,                                                                               ",
        "      ,/   .`|                                                                              ",
        "    ,`   .'  :  ,---,                                           ___                        ",
        "  ;    ;     /,--.' |                                         ,--.'|_                      ",
        ".'___,/    ,' |  |  :                      ,---,              |  | :,'   ,---.             ",
        "|    :     |  :  :  :                  ,-+-. /  |             :  : ' :  '   ,'\\   .--.--.  ",
        ";    |.';  ;  :  |  |,--.  ,--.--.    ,--.'|'   |  ,--.--.  .;__,'  /  /   /   | /  /    ' ",
        "`----'  |  |  |  :  '   | /       \\  |   |  ,\"' | /       \\ |  |   |  .   ; ,. :|  :  /`./ ",
        "    '   :  ;  |  |   /' :.--.  .-. | |   | /  | |.--.  .-. |:__,'| :  '   | |: :|  :  ;_   ",
        "    |   |  '  '  :  | | | \\__\\/: . . |   | |  | | \\__\\/: . .  '  : |__'   | .; : \\  \\    `. ",
        "    '   :  |  |  |  ' | : ,\" .--.; | |   | |  |/  ,\" .--.; |  |  | '.'|   :    |  `----.   \\",
        "    ;   |.'   |  :  :_:,'/  /  ,.  | |   | |--'  /  /  ,.  |  ;  :    ;\\   \\  /  /  /`--'  / ",
        "    '---'     |  | ,'   ;  :   .'   \\|   |/     ;  :   .'   \\ |  ,   /  `----'  '--'.     /  ",
        "              `--''     |  ,     .-./'---'      |  ,     .-./  ---`-'             `--'---'   ",
        "                         `--`---'                `--`---'                                  ",
        "                                                                                           "
    };
    int colors[] = {0x0C,0x0E,0x0A,0x0B,0x09,0x0D,0x0F};
    int color_count = 7;
    for (int i = 0; i < (int)(sizeof(lines)/sizeof(lines[0])); ++i) {
        int color = colors[i % color_count];
        print_color(lines[i], color);
        print("\n");
    }
    print("\n");
}

static char command_buffer[128];
static int command_len = 0;
static bool prompt_shown = false;

// Edit mode variables
static bool edit_mode = false;
static int editing_file_index = -1;
static char edit_buffer[MAX_FILE_CONTENT];
static int edit_pos = 0;

void print_prompt(void) {
    if (!prompt_shown) {
        print("ThanatOS $ ");
        print(current_dir);
        print("> ");
        prompt_shown = true;
    }
}

void show_cursor(void) {
    // No cursor underscore
}

void clear_command_buffer(void) {
    command_buffer[0] = '\0';
    command_len = 0;
}

void start_edit_mode(int file_index) {
    edit_mode = true;
    editing_file_index = file_index;
    // Copy current content to edit buffer
    strcpy(edit_buffer, file_table[file_index].content);
    edit_pos = file_table[file_index].content_length;
    print("Entering edit mode for file: ");
    print(file_table[file_index].name);
    print("\nType your text. Press Ctrl+S to save and exit, Ctrl+Q to quit without saving.\n");
    print("Current content:\n");
    if (edit_pos > 0) {
        print(edit_buffer);
    } else {
        print("(empty file)\n");
    }
    print("\n> ");
}

void stop_edit_mode(bool save) {
    if (save && editing_file_index >= 0) {
        // Save the content
        strcpy(file_table[editing_file_index].content, edit_buffer);
        file_table[editing_file_index].content_length = edit_pos;
        print("File saved.\n");
    } else {
        print("Edit cancelled.\n");
    }
    edit_mode = false;
    editing_file_index = -1;
    edit_buffer[0] = '\0';
    edit_pos = 0;
}

void save_user_config(const char* username, const char* password, int keyboard_layout) {
    // Create user directory
    create_dir("user");

    // Create accounts file with user credentials
    char accounts_path[] = "user/accounts.txt";
    if (create_file(accounts_path)) {
        // Find the file we just created and write the account info
        for (int i = 0; i < MAX_FILES; ++i) {
            if (file_table[i].exists && strcmp(file_table[i].name, accounts_path) == 0) {
                // Format: username:password:keyboard_layout
                int len = 0;
                strcpy(file_table[i].content, username);
                len = strlen(username);
                file_table[i].content[len] = ':';
                len++;
                strcpy(file_table[i].content + len, password);
                len += strlen(password);
                file_table[i].content[len] = ':';
                len++;
                file_table[i].content[len] = '0' + keyboard_layout;
                len++;
                file_table[i].content[len] = '\0';
                file_table[i].content_length = len;
                break;
            }
        }
    }
}

bool load_user_config(char* username, char* password, int* keyboard_layout) {
    char accounts_path[] = "user/accounts.txt";

    // Find the accounts file
    for (int i = 0; i < MAX_FILES; ++i) {
        if (file_table[i].exists && strcmp(file_table[i].name, accounts_path) == 0) {
            // Parse the content: username:password:keyboard_layout
            char* content = file_table[i].content;
            char* colon1 = strchr(content, ':');
            if (!colon1) return false;

            char* colon2 = strchr(colon1 + 1, ':');
            if (!colon2) return false;

            // Extract username
            int username_len = colon1 - content;
            if (username_len >= MAX_USERNAME_LEN) return false;
            strncpy(username, content, username_len);
            username[username_len] = '\0';

            // Extract password
            int password_len = colon2 - (colon1 + 1);
            if (password_len >= MAX_PASSWORD_LEN) return false;
            strncpy(password, colon1 + 1, password_len);
            password[password_len] = '\0';

            // Extract keyboard layout
            *keyboard_layout = *(colon2 + 1) - '0';

            return true;
        }
    }
    return false;
}

void handle_input(char c) {
    // Handle edit mode
    if (edit_mode) {
        if (c == 19) { // Ctrl+S (save) - ASCII 19 is Ctrl+S
            stop_edit_mode(true);
            print_prompt();
            return;
        } else if (c == 17) { // Ctrl+Q (quit) - ASCII 17 is Ctrl+Q
            stop_edit_mode(false);
            print_prompt();
            return;
        } else if (c == '\n' || c == '\r') {
            // Add newline to edit buffer
            if (edit_pos < MAX_FILE_CONTENT - 1) {
                edit_buffer[edit_pos++] = '\n';
                edit_buffer[edit_pos] = '\0';
                print("\n> ");
            }
            return;
        } else if (c == '\b' && edit_pos > 0) {
            edit_pos--;
            edit_buffer[edit_pos] = '\0';
            print("\b \b");
            return;
        } else if (c >= 32 && c <= 126 && edit_pos < MAX_FILE_CONTENT - 1) {
            edit_buffer[edit_pos++] = c;
            edit_buffer[edit_pos] = '\0';
            char buf[2] = {c, 0};
            print(buf);
            return;
        }
        return;
    }

    print_prompt();
    if (c == PAGE_UP) {
        scroll_up();
        return;
    } else if (c == PAGE_DOWN) {
        scroll_down();
        return;
    } else if (c == '\n' || c == '\r') {
        print("\n");
        command_buffer[command_len] = '\0';
        prompt_shown = false;

        if (strcmp(command_buffer, "-v") == 0) {
            print(VERSION_STRING);
        } else if (strcmp(command_buffer, "clear") == 0) {
            print(VERSION_STRING);
        } else if (strcmp(command_buffer, "clear") == 0) {
            clear_screen();
            prompt_shown = false;
            clear_command_buffer();
            print_prompt();
            show_cursor();
            return;
        } else if (strcmp(command_buffer, "azerty") == 0) {
            set_keyboard_layout(LAYOUT_AZERTY);
            print("Keyboard layout set to AZERTY.\n");
        } else if (strcmp(command_buffer, "exit") == 0 || strcmp(command_buffer, "quit") == 0) {
            print("Exiting console...\n");
            running = false;
            return;
        } else if (strcmp(command_buffer, "shutdown") == 0) {
            print("Shutting down...\n");
            system_action = 1;
            running = false;
            return;
        } else if (strcmp(command_buffer, "reboot") == 0) {
            print("Rebooting...\n");
            system_action = 2;
            running = false;
            return;
        } else if (strcmp(command_buffer, "dir") == 0 || strcmp(command_buffer, "ls") == 0) {
            list_files();
        } else if (strncmp(command_buffer, "mkf ", 4) == 0) {
            const char* filename = command_buffer + 4;
            if (strlen(filename) == 0) {
                print("Usage: mkf <filename\n");
            } else if (create_file(filename)) {
                print("File created: ");
                print(filename);
                print("\n");
            } else {
                print("Error: file table full.\n");
            }
        } else if (strncmp(command_buffer, "rmf ", 4) == 0) {
            const char* filename = command_buffer + 4;
            if (strlen(filename) == 0) {
                print("Usage: rmf <filename\n");
            } else {
                // Check if it's a system file
                char full_path[MAX_FILENAME_LEN];
                if (strcmp(current_dir, "/") == 0) {
                    strncpy(full_path, filename, MAX_FILENAME_LEN - 1);
                } else {
                    int len = strlen(current_dir);
                    if (len + 1 + strlen(filename) >= MAX_FILENAME_LEN) {
                        print("Path too long.\n");
                        clear_command_buffer();
                        print_prompt();
                        show_cursor();
                        return;
                    }
                    strcpy(full_path, current_dir);
                    full_path[len] = '/';
                    strcpy(full_path + len + 1, filename);
                }
                full_path[MAX_FILENAME_LEN - 1] = '\0';

                if (is_system_file(full_path)) {
                    print("Cannot remove system file: ");
                    print(filename);
                    print("\n");
                } else if (remove_file(filename)) {
                    print("File removed: ");
                    print(filename);
                    print("\n");
                } else {
                    print("File not found.\n");
                }
            }
                } else if (strncmp(command_buffer, "rnm ", 4) == 0) {
            const char* args = command_buffer + 4;
            char* space = NULL;
            for (char* p = (char*)args; *p; p++) {
                if (*p == ' ') {
                    space = p;
                    break;
                }
            }
            if (!space || space == (char*)args) {
                print("Usage: rnm <oldname <newname\n");
            } else {
                *space = '\0';
                const char* oldname = args;
                const char* newname = space + 1;
                if (strlen(newname) == 0) {
                    print("Usage: rnm <oldname <newname\n");
                } else {
                    // Check if oldname is a system file
                    char full_path[MAX_FILENAME_LEN];
                    if (strcmp(current_dir, "/") == 0) {
                        strncpy(full_path, oldname, MAX_FILENAME_LEN - 1);
                    } else {
                        int len = strlen(current_dir);
                        if (len + 1 + strlen(oldname) >= MAX_FILENAME_LEN) {
                            print("Path too long.\n");
                            clear_command_buffer();
                            print_prompt();
                            show_cursor();
                            return;
                        }
                        strcpy(full_path, current_dir);
                        full_path[len] = '/';
                        strcpy(full_path + len + 1, oldname);
                    }
                    full_path[MAX_FILENAME_LEN - 1] = '\0';

                    if (is_system_file(full_path)) {
                        print("Cannot rename system file: ");
                        print(oldname);
                        print("\n");
                    } else if (rename_file(oldname, newname)) {
                        print("Renamed ");
                        print(oldname);
                        print(" to ");
                        print(newname);
                        print("\n");
                    } else {
                        print("Error: file not found or new name already exists.\n");
                    }
                }
            }
        } else if (strncmp(command_buffer, "mkdir ", 6) == 0) {
            const char* dirname = command_buffer + 6;
            if (strlen(dirname) == 0) {
                print("Usage: mkdir <dirname>\n");
            } else if (create_dir(dirname)) {
                print("Directory created: ");
                print(dirname);
                print("\n");
            } else {
                print("Error: directory table full.\n");
            }
        } else if (strncmp(command_buffer, "cat ", 4) == 0) {
            const char* filename = command_buffer + 4;
            if (strlen(filename) == 0) {
                print("Usage: cat <filename>\n");
            } else {
                // Find the file
                char full_path[MAX_FILENAME_LEN];
                if (strcmp(current_dir, "/") == 0) {
                    strncpy(full_path, filename, MAX_FILENAME_LEN - 1);
                } else {
                    int len = strlen(current_dir);
                    if (len + 1 + strlen(filename) >= MAX_FILENAME_LEN) {
                        print("Path too long.\n");
                        clear_command_buffer();
                        print_prompt();
                        show_cursor();
                        return;
                    }
                    strcpy(full_path, current_dir);
                    full_path[len] = '/';
                    strcpy(full_path + len + 1, filename);
                }
                full_path[MAX_FILENAME_LEN - 1] = '\0';

                bool found = false;
                int file_index = -1;
                for (int i = 0; i < MAX_FILES; ++i) {
                    if (file_table[i].exists && strcmp(file_table[i].name, full_path) == 0) {
                        found = true;
                        file_index = i;
                        break;
                    }
                }

                if (found) {
                    if (file_table[file_index].content_length > 0) {
                        print("Content of ");
                        print(filename);
                        print(":\n");
                        print(file_table[file_index].content);
                        print("\n");
                    } else {
                        print("File is empty: ");
                        print(filename);
                        print("\n");
                    }
                } else {
                    print("File not found: ");
                    print(filename);
                    print("\n");
                }
            }
        } else if (strncmp(command_buffer, "edit ", 5) == 0) {
            const char* filename = command_buffer + 5;
            if (strlen(filename) == 0) {
                print("Usage: edit <filename>\n");
            } else {
                // Find the file
                char full_path[MAX_FILENAME_LEN];
                if (strcmp(current_dir, "/") == 0) {
                    strncpy(full_path, filename, MAX_FILENAME_LEN - 1);
                } else {
                    int len = strlen(current_dir);
                    if (len + 1 + strlen(filename) >= MAX_FILENAME_LEN) {
                        print("Path too long.\n");
                        clear_command_buffer();
                        print_prompt();
                        show_cursor();
                        return;
                    }
                    strcpy(full_path, current_dir);
                    full_path[len] = '/';
                    strcpy(full_path + len + 1, filename);
                }
                full_path[MAX_FILENAME_LEN - 1] = '\0';

                bool found = false;
                int file_index = -1;
                for (int i = 0; i < MAX_FILES; ++i) {
                    if (file_table[i].exists && strcmp(file_table[i].name, full_path) == 0) {
                        found = true;
                        file_index = i;
                        break;
                    }
                }

                if (found) {
                    // Check if it's a system file
                    if (is_system_file(full_path)) {
                        print("Cannot edit system file: ");
                        print(filename);
                        print("\n");
                    } else {
                        start_edit_mode(file_index);
                    }
                } else {
                    print("File not found: ");
                    print(filename);
                    print("\n");
                }
            }
        } else if (strcmp(command_buffer, "cd") == 0) {
            print("Current directory: ");
            print(current_dir);
            print("\n");
        } else if (strncmp(command_buffer, "cd ", 3) == 0) {
            const char* path = command_buffer + 3;
            if (strcmp(path, "/") == 0) {
                strcpy(current_dir, "/");
                print("Changed to /\n");
            } else if (strcmp(path, "..") == 0) {
                // Go up one directory
                if (strcmp(current_dir, "/") != 0) {
                    char* last_slash = strrchr(current_dir, '/');
                    if (last_slash != current_dir) {
                        *last_slash = '\0';
                    } else {
                        strcpy(current_dir, "/");
                    }
                    print("Changed to ");
                    print(current_dir);
                    print("\n");
                } else {
                    print("Already at root directory.\n");
                }
            } else {
                // Check if it's a relative path or absolute path
                char target_path[MAX_FILENAME_LEN];
                if (path[0] == '/') {
                    // Absolute path
                    strncpy(target_path, path, MAX_FILENAME_LEN - 1);
                } else {
                    // Relative path
                    if (strcmp(current_dir, "/") == 0) {
                        strncpy(target_path, path, MAX_FILENAME_LEN - 1);
                    } else {
                        int len = strlen(current_dir);
                        if (len + 1 + strlen(path) >= MAX_FILENAME_LEN) {
                            print("Path too long.\n");
                        } else {
                            strcpy(target_path, current_dir);
                            target_path[len] = '/';
                            strcpy(target_path + len + 1, path);
                        }
                    }
                }
                target_path[MAX_FILENAME_LEN - 1] = '\0';

                // Check if directory exists
                bool found = false;
                for (int i = 0; i < MAX_DIRS; ++i) {
                    if (dir_table[i].exists && strcmp(dir_table[i].name, target_path) == 0) {
                        found = true;
                        break;
                    }
                }

                if (found) {
                    strcpy(current_dir, target_path);
                    print("Changed to ");
                    print(current_dir);
                    print("\n");
                } else {
                    print("Directory not found: ");
                    print(path);
                    print("\n");
                }
            }
        } else if (command_len > 0) {
            print("Unknown command: ");
            print(command_buffer);
            print("\n");
        }

        clear_command_buffer();
        print_prompt();
        show_cursor();
        return;
    }

    if (c == '\b' && command_len > 0) {
        command_len--;
        command_buffer[command_len] = '\0';
        print("\b \b");
        return;
    }

    if (c >= 32 && c <= 126 && command_len < (int)sizeof(command_buffer) - 1) {
        command_buffer[command_len++] = c;
        char buf[2] = {c, 0};
        print(buf);
    }

    show_cursor();
}
