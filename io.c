#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "io.h"
#include "serial.h"
#include "str.h"

/* The I/O ports */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

/* Hardware text mode color constants. */
enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

/** vga_entry_color:
 *  Returns an 8 bit value which represents a foreground and background
 *
 *  @param fg the foreground color
 *  @param bg the background color
 */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
  return fg | bg << 4;
}

/** vga_entry:
 *  Returns a 16 bit value which represents a foreground and background color
 * and a value to write
 *
 *  @param uc value to write as an unsigned char
 *  @param the color to write
 */
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t framebuffer_row;
size_t framebuffer_column;
uint8_t framebuffer_color;
uint16_t *framebuffer_buffer;

/** framebuffer_initialize:
 *  Initializes a framebuffer
 *
 */
void framebuffer_initialize(void) {
  framebuffer_row = 0;
  framebuffer_column = 0;
  framebuffer_color = vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_WHITE);
  framebuffer_buffer = (uint16_t *)0xB8000;
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      framebuffer_buffer[index] = vga_entry(' ', framebuffer_color);
    }
  }
}

/** framebuffer_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void framebuffer_move_cursor(unsigned short pos) {
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  outb(FB_DATA_PORT, pos & 0x00FF);
}

/** framebuffer_setcolor:
 *  Sets the framebuffer color
 *
 *  @param color The color to set
 */
void framebuffer_setcolor(uint8_t color) { framebuffer_color = color; }

/** framebuffer_putentryat:
 *  Puts an entry in the framebuffer
 *
 *  @param c The character to put in the framebuffer
 *  @param color The color to use
 */
void framebuffer_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  framebuffer_buffer[index] = vga_entry(c, color);
}

void framebuffer_clearline(size_t row) {
  for (size_t i = 0; i < VGA_WIDTH; i++) {
    framebuffer_putentryat(0, framebuffer_color, i, row);
  }
}

/** framebuffer_putchar:
 *  Puts a character in the framebuffer
 *
 *  @param c the character to put
 */
void framebuffer_putchar(char c) {
  if (c == 0x0a) {
    framebuffer_newline();
  } else {
    framebuffer_putentryat(c, framebuffer_color, framebuffer_column,
                           framebuffer_row);

    if (++framebuffer_column == VGA_WIDTH) {
      framebuffer_column = 0;
      if (++framebuffer_row == VGA_HEIGHT) {
        framebuffer_row = 0;
      }
    }

    framebuffer_move_cursor((framebuffer_row)*VGA_WIDTH + framebuffer_column);
  }
}

/** framebuffer_newline:
 *  Moves the cursor to a new line
 *
 */
void framebuffer_newline(void) {
  if (++framebuffer_row == VGA_HEIGHT) {
    framebuffer_row = 0;
  }

  framebuffer_clearline(framebuffer_row);

  framebuffer_column = 0;

  framebuffer_move_cursor(framebuffer_row * framebuffer_column);
}

/** framebuffer_write:
 *  Writes data to the framebuffer
 *
 *  @param data a pointer to the start of the data to write
 *  @param size the number of bytes to write
 */
void framebuffer_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++) {
    framebuffer_putchar(data[i]);
  }
}

/** framebuffer_writestring:
 *  Writes a null terminated string to the framebuffer
 *
 *  @param data a pointer to the start of the string to write
 */
void framebuffer_writestring(const char *data) {
  framebuffer_write(data, strlen(data));
}

/** framebuffer_writeline:
 *  Writes a null terminated string to the framebuffer and moves to the next
 * line
 *
 *  @param data a pointer to the start of the string to write
 */
void framebuffer_writeline(const char *data) {
  framebuffer_write(data, strlen(data));
  framebuffer_newline();
}

/** fprintf:
 *  Writes a null terminated string to the output specified
 *
 *  @param output the device to write output to
 *  @param string pointer to the string to write
 */
void fprintf(unsigned short output, const char *string, ...) {
  size_t i;
  va_list args;
  char formatted[strlen(string) + 1];

  size_t val_count = format_param_count(string);

  uint8_t vals[val_count];

  va_start(args, val_count);

  for (i = 0; i < val_count; i++) {
    vals[i] = va_arg(args, int);
  }

  va_end(args);

  format_string(formatted, string, vals);

  if (output == SERIAL) {
    serial_writestring(SERIAL_COM1_BASE, formatted);
  } else if (output == FRAMEBUFFER) {
    framebuffer_writestring(formatted);
  }
}
