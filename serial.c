#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "io.h"
#include "serial.h"
#include "str.h"

/* The I/O ports */

/** serial_configure_baud_rate:
 *  Sets the speed of the data being sent. The default speed of a serial
 *  port is 115200 bits/s. The argument is a divisor of that number, hence
 *  the resulting speed becomes (115200 / divisor) bits/s.
 *
 *  @param com      The COM port to configure
 *  @param divisor  The divisor
 */
void serial_configure_baud_rate(unsigned short com, unsigned short divisor) {
  outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
  outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
  outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);
}

/** serial_configure_line:
 *  Configures the line of the given serial port. The port is set to have a
 *  data length of 8 bits, no parity bits, one stop bit and break control
 *  disabled.
 *
 *  @param com  The serial port to configure
 */
void serial_configure_line(unsigned short com) {
  /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
   * Content: | d | b | prty  | s | dl  |
   * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
   */
  outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

/** serial_configure_buffers:
 *  Configures the serial modem buffers.
 *  We're setting it to use 14 bytes for buffer,
 *  clear the input and output queues and enable FIFO
 *
 *  @param com  The serial port to configure
 */
void serial_configure_buffers(unsigned short com) {
  /** Bit:     | 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
    * Content: | lvl | bs | r | dma | clt | clr | e |
    */
  outb(SERIAL_FIFO_COMMAND_PORT(com), 0xc7);
}

/** serial_configure_modem:
 *  All we need to do is set rts and dts/dtr
 *
 *  @param com  The serial port to configure
 */
void serial_configure_modem(unsigned short com) {
  /** Bit:     | 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
    * Content: | r | r | af | lb | ao2 | ao1 | rts | dtr |
    */
  outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

/** serial_initialize:
 *  initializes serial modem
 *
 *  @param com  The serial port to configure
 *  @param divisor  The divisor to use
 */
void serial_initialize(unsigned short com, unsigned short divisor) {
  serial_configure_baud_rate(com, divisor);
  serial_configure_line(com);
  serial_configure_buffers(com);
  serial_configure_modem(com);
}

/** serial_is_transmit_fifo_empty:
 *  Checks whether the transmit FIFO queue is empty or not for the given COM
 *  port.
 *
 *  @param  com The COM port
 *  @return 0 if the transmit FIFO queue is not empty
 *          1 if the transmit FIFO queue is empty
 */
int serial_is_transmit_fifo_empty(unsigned int com) {
  /* 0x20 = 0010 0000 */
  return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

/** serial_write:
 *  Writes data to the serial port
 *
 *  @param com the COM port
 *  @param data a pointer to the start of the data to write
 *  @param size the number of bytes to write
 */
void serial_write(unsigned int com, const char *data, size_t size) {
  size_t count = 0;
  while (count < size) {
    if (serial_is_transmit_fifo_empty(com)) {
      outb(SERIAL_DATA_PORT(com), data[count]);
      count++;
    }
  }
}
/** serial_writestring:
 *  Writes a null terminated string to the serial port
 *
 *  @param com the COM port
 *  @param data a pointer to the start of the string to write
 */
void serial_writestring(unsigned int com, const char *data) {
  serial_write(com, data, strlen(data));
}
