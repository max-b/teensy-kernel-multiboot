#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"
#include "io.h"
#include "serial.h"
#include "gdt.h"

/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error                                                                         \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(void) {
  /* Initialize framebuffer */
  framebuffer_initialize();

  gdt_init();
  idt_init();

  framebuffer_writeline("Helloooooo kernel world");
  framebuffer_writeline("Now we can even read from the keyboard :)");

  serial_initialize(SERIAL_COM1_BASE, 1);
  serial_writestring(SERIAL_COM1_BASE, "Helloooo serial port?");

  fprintf(SERIAL, "printing to serial\n");
  fprintf(FRAMEBUFFER, "printing to framebuffer");

  fprintf(FRAMEBUFFER, "printing a format string: %%\n", 0x11);
  fprintf(SERIAL, "printing a format string: %%\n", 0x11);

  for (;;) {
    asm("hlt");
  }
}
