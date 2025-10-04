#include <string.h>
#include <stdint.h>
#include "video.h"

#define SCROLLBACK_LINES 1000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

static char text_buffer[SCROLLBACK_LINES][VGA_WIDTH + 1];
static int buffer_head = 0;
static int buffer_count = 0;
static int scroll_offset = 0;
static char current_line[VGA_WIDTH + 1];
static int line_pos = 0;

static uint16_t* vga_buffer = (uint16_t*) VGA_ADDRESS;
static int cursor_x = 0;
static int cursor_y = 0;

static uint8_t current_color = COLOR_LIGHT_GREY;

static uint16_t make_vga_entry(char c, uint8_t color) {
    return ((uint16_t)color << 8) | c;
}

void add_line_to_buffer(const char* line) {
    strcpy(text_buffer[buffer_head], line);
    buffer_head = (buffer_head + 1) % SCROLLBACK_LINES;
    if (buffer_count < SCROLLBACK_LINES) buffer_count++;
}

void redraw_screen() {
    int start_line = (buffer_head - VGA_HEIGHT + SCROLLBACK_LINES - scroll_offset) % SCROLLBACK_LINES;
    for (int y = 0; y < VGA_HEIGHT; y++) {
        int line_idx = (start_line + y) % SCROLLBACK_LINES;
        for (int x = 0; x < VGA_WIDTH; x++) {
            char c = text_buffer[line_idx][x];
            if (c == '\0') c = ' ';
            vga_buffer[y * VGA_WIDTH + x] = make_vga_entry(c, current_color);
        }
    }
    cursor_y = VGA_HEIGHT - 1;
    cursor_x = line_pos;
}

void scroll_up() {
    if (scroll_offset < buffer_count - VGA_HEIGHT) {
        scroll_offset++;
        redraw_screen();
    }
}

void scroll_down() {
    if (scroll_offset > 0) {
        scroll_offset--;
        redraw_screen();
    }
}

void set_text_color(uint8_t color) {
    current_color = color;
}

void reset_text_color() {
    set_text_color(COLOR_LIGHT_GREY);
}

void clear_screen() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = make_vga_entry(' ', current_color);
        }
    }
    cursor_x = 0;
    cursor_y = 0;
    line_pos = 0;
    buffer_head = 0;
    buffer_count = 0;
    scroll_offset = 0;
}

void putchar_color(char c) {
    if (c == '\n') {
        current_line[line_pos] = '\0';
        add_line_to_buffer(current_line);
        line_pos = 0;
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = make_vga_entry(' ', current_color);
            if (line_pos > 0) line_pos--;
        }
    } else if (c >= 32 && c <= 126) {
        current_line[line_pos++] = c;
        if (line_pos >= VGA_WIDTH) line_pos = VGA_WIDTH - 1;
        vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = make_vga_entry(c, current_color);
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= VGA_HEIGHT) {
        if (scroll_offset > 0) {
            scroll_offset = 0;
            redraw_screen();
        } else {
            // Scroll
            for (int y = 1; y < VGA_HEIGHT; y++) {
                for (int x = 0; x < VGA_WIDTH; x++) {
                    vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
                }
            }
            // Effacer dernière ligne
            for (int x = 0; x < VGA_WIDTH; x++) {
                vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = make_vga_entry(' ', current_color);
            }
        }
        cursor_y = VGA_HEIGHT - 1;
    }
}

void print_color(const char* str, uint8_t color) {
    uint8_t saved = current_color;
    set_text_color(color);
    while (*str)
        putchar_color(*str++);
    set_text_color(saved);
}
