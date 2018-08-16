#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "interrupts.h"
#include "io.h"
#include "constants.h"

idt_entry_t idt_entries[IDT_NUM_ENTRIES];

void interrupt_handler_0(void);
void interrupt_handler_1(void);
void interrupt_handler_2(void);
void interrupt_handler_3(void);
void interrupt_handler_4(void);
void interrupt_handler_5(void);
void interrupt_handler_6(void);
void interrupt_handler_7(void);
void interrupt_handler_8(void);
void interrupt_handler_9(void);
void interrupt_handler_10(void);
void interrupt_handler_11(void);
void interrupt_handler_12(void);
void interrupt_handler_13(void);
void interrupt_handler_14(void);
void interrupt_handler_15(void);
void interrupt_handler_16(void);
void interrupt_handler_17(void);
void interrupt_handler_18(void);
void interrupt_handler_19(void);

void interrupt_handler_32(void);
void interrupt_handler_33(void);

void pic_acknowledge(void) {
  outb(PIC1_PORT_A, PIC_EOI);
  outb(PIC2_PORT_A, PIC_EOI);
}

void interrupt_handler(__attribute__((unused)) cpu_state_t cpu, __attribute__((unused)) idt_info_t info, __attribute__((unused)) stack_state_t stack) {
  unsigned char scan_code;
  char *message = "key: _\n";

  fprintf(SERIAL, "interrupt handler number: %%\n", info.idt_index);

  switch (info.idt_index) {
    case 33:
      scan_code = inb(0x60);
      message[5] = scan_code;
      fprintf(SERIAL, "interrupt number %%, key: %%\n", 33, scan_code);
      fprintf(FRAMEBUFFER, "key: %%\n", scan_code);
      if (scan_code == 0x50) {
        fprintf(SERIAL, "down\n");
        fprintf(FRAMEBUFFER, "down\n");
      } else if (scan_code == 0x48) {
        fprintf(SERIAL, "up\n");
        fprintf(FRAMEBUFFER, "up\n");

      }

      break;
    default:
      fprintf(SERIAL, "interrupt number not in list\n");
      break;
  }

  if (info.idt_index >= 0x20 && info.idt_index <= 0x2f) {
    pic_acknowledge();
  }
}

void set_idt_entry(unsigned int n, uint32_t handler, unsigned int type, unsigned int privilege) {
  idt_entries[n] = (idt_entry_t) {
    .offset_low=handler & 0x0000ffff,
    .selector=SEGSEL_KERNEL_CS,
    .unused=0x0,
    /* type is either interrupt or trap depending on last bit */
    .type=0b1110 | type,
    .attributes=0b1000 | (privilege << 1),
    .offset_high=handler >> 16,
  };
}

/* from https://www-s.acm.illinois.edu/sigops/2007/roll_your_own/i386/irq.html */
void init_pic(void) {
  /* ICW1 */
  outb(PIC1_PORT_A, PIC1_ICW1); /* Master port A */
  outb(PIC2_PORT_A, PIC2_ICW1); /* Slave port A */

  /* ICW2 */
  outb(PIC1_PORT_B, PIC1_ICW2); /* Master offset of 0x20 in the IDT */
  outb(PIC2_PORT_B, PIC2_ICW2); /* Master offset of 0x28 in the IDT */

  /* ICW3 */
  outb(PIC1_PORT_B, PIC1_ICW3); /* Slaves attached to IR line 2 */
  outb(PIC2_PORT_B, PIC2_ICW3); /* This slave in IR line 2 of master */

  /* ICW4 */
  outb(PIC1_PORT_B, PIC1_ICW4); /* Set as master */
  outb(PIC2_PORT_B, PIC2_ICW4); /* Set as slave */

  // TODO: do I need to setup pic mask?
  // pic_mask(0xec, 0xff);
  outb(PIC1_PORT_B, 0xfd);
  /* outb(PIC1_PORT_B, 0xfc); // to enable timer */
  outb(PIC2_PORT_B, 0xff);
}

void idt_init(void) {
  idt_ptr_t idt_ptr;
  idt_ptr.limit = IDT_NUM_ENTRIES * sizeof(idt_entry_t) - 1;
  idt_ptr.base = (uint32_t) &idt_entries;

  set_idt_entry(0, (uint32_t) &interrupt_handler_0, IDT_INTERRUPT_GATE_TYPE, PL0);
  set_idt_entry(1, (uint32_t) &interrupt_handler_1, IDT_INTERRUPT_GATE_TYPE, PL0);

  set_idt_entry(IDT_TIMER_INTERRUPT_INDEX, (uint32_t) &interrupt_handler_32, IDT_INTERRUPT_GATE_TYPE, PL0);
  set_idt_entry(IDT_KEYBOARD_INTERRUPT_INDEX, (uint32_t) &interrupt_handler_33, IDT_INTERRUPT_GATE_TYPE, PL0);

  load_idt((uint32_t) &idt_ptr);

  init_pic();

  enable_interrupts();
}
