#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#include <stddef.h>
#include <stdint.h>

#define SERIAL 0
#define FRAMEBUFFER 1

/** outb:
 *  Sends the given data to the given I/O port. Defined in io.asm
 *
 *  @param port The I/O port to send the data to
 *  @param data The data to send to the I/O port
 */
void outb(unsigned short port, unsigned char data);

/** inb:
 *  Read a byte from an I/O port. Defined in io.asm
 *
 *  @param  port The address of the I/O port
 *  @return      The read byte
 */
unsigned char inb(unsigned short port);

void framebuffer_initialize(void);
void framebuffer_move_cursor(unsigned short pos);
void framebuffer_writestring(const char* data);
void framebuffer_writeline(const char* data);
void framebuffer_newline(void);

void fprintf(unsigned short output, const char* string, ...);

#endif /* INCLUDE_IO_H */
