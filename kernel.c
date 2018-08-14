#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "io.h"
#include "serial.h"
#include "tables.h"
#include "interrupts.h"

/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif


void kernel_main(void)
{
  /* Initialize framebuffer */
  framebuffer_initialize();

  gdt_init();
  idt_init();

  framebuffer_writeline("newline??");
  framebuffer_writestring("Helloooooo kernel world");

  serial_initialize(SERIAL_COM1_BASE, 1);
  serial_writestring(SERIAL_COM1_BASE, "Helloooo serial port?");

  fprintf(SERIAL, "printing to serial\n");
  fprintf(FRAMEBUFFER, "printing to framebuffer");

  for(;;) {
    asm("hlt");
  }
}
