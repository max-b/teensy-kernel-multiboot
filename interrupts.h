#ifndef INCLUDE_INTERRUPTS_H
#define INCLUDE_INTERRUPTS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#define IDT_INTERRUPT_GATE_TYPE 0
#define IDT_TRAP_GATE_TYPE 1

#define IDT_TIMER_INTERRUPT_INDEX    0x20
#define IDT_KEYBOARD_INTERRUPT_INDEX 0x21

#define IDT_NUM_ENTRIES 48

#define PIC1_PORT_A 0x20
#define PIC1_PORT_B 0x21

#define PIC2_PORT_A 0xA0
#define PIC2_PORT_B 0xA1

#define PIC1_ICW1   0x11 /* Initialize the PIC and enable ICW4 */
#define PIC2_ICW1   PIC1_ICW2

#define PIC1_ICW2   0x20 /* IRQ 0-7 will be remapped to IDT index 32 - 39 */
#define PIC2_ICW2   0x28 /* IRQ 8-15 will be remapped to IDT index 40 - 47 */

#define PIC1_ICW3   0x04 /* PIC1 is connected to PIC2 via IRQ2 */
#define PIC2_ICW3   0x02 /* PIC2 is connected to PIC1 via IRQ1 */

#define PIC1_ICW4   0x05 /* 8086/88 mode is enabled and PIC1 is master */
#define PIC2_ICW4   0x01 /* 8086/88 mode is enabled */

#define PIC_EOI 0x20

struct idt_entry {
  uint16_t offset_low; /* lowest part of interrupt function's offset address  */
  uint16_t selector; /* code segment selector */
  uint8_t unused; /* unused - have to be zero */
  unsigned int type : 4; /* gate types */
  /* Attributes nibble
   * storage segment: 1bit (0 for interrupt and trap gates); DPL (call gate protection): 2bits
   * Present (set to 0 for unused interrupts): 1 bit
   */
  unsigned int attributes: 4;
  uint16_t offset_high; /* The high 16 bits of the offset */
} __attribute__((packed));  /* It needs to be packed like this, 64 bits */

typedef struct idt_entry idt_entry_t;

struct idt_ptr {
  uint16_t limit;          /* Size of idt table in bytes*/
  uint32_t base;           /* Address to the first idt entry */
} __attribute__((packed));

typedef struct idt_ptr idt_ptr_t;

struct idt_info {
  uint32_t idt_index;
  uint32_t error_code;
} __attribute__((packed));

typedef struct idt_info idt_info_t;

struct cpu_state {
  unsigned int eax;
  unsigned int ebx;
  unsigned int ecx;
  unsigned int edx;
  unsigned int esp;
} __attribute__((packed));

typedef struct cpu_state cpu_state_t;

struct stack_state {
  unsigned int error_code;
  unsigned int eip;
  unsigned int cs;
  unsigned int eflags;
} __attribute__((packed));

typedef struct stack_state stack_state_t;

void interrupt_handler(cpu_state_t cpu, idt_info_t info, stack_state_t stack);

void load_idt(uint32_t address);

void enable_interrupts(void);
void disable_interrupts(void);

void idt_init(void);

#endif /* INCLUDE_INTERRUPTS_H */
