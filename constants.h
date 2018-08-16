#ifndef INCLUDE_CONSTANTS_H
#define INCLUDE_CONSTANTS_H

/* Privilege levels */
#define PL0 0x00
#define PL3 0x03

/* Segment selectors in our gdt */
#define SEGSEL_KERNEL_CS 0x08
#define SEGSEL_KERNEL_DS 0x10

#endif
